#pragma once

#include <complex>
#include <fstream>      // fstream
#include <iostream>     // iostream
#include <memory>       // unique_ptr
#include <stdexcept>    // runtime_error
#include "ndarray.hpp"

namespace cnumpy {

    class NPY {
    public:
        NPY(const std::string &filename, const char mode) :
                mode_(mode), iostrm_(std::cout.rdbuf()) {
            auto iosmode = std::ios::binary;
            switch (mode_) {
                case 'r':
                    iosmode |= std::ios::in;
                    break;
                case 'w':
                    iosmode |= std::ios::out;
                    break;
                default:
                    throw std::runtime_error("NPY::NPY(): unexpected mode");
            }
            fstrm_.open(filename, iosmode);
            if (fstrm_.fail())
                throw std::runtime_error("NPY::NPY(): can't open file");
            iostrm_.rdbuf(fstrm_.rdbuf());
        }

        void close() {
            mode_ = 0;
            fstrm_.close();
        }

        template<class T>
        ndarray <T> load() {
            if (mode_ && mode_ != 'r')
                throw std::runtime_error("NPY::load(): file not opened in 'r' mode");

            auto read_stream = []<class T_>(std::iostream &iostrm, T_ &out) {
                char buffer[sizeof(T_)];
                if (iostrm.read(buffer, sizeof(T_)).fail())
                    throw std::runtime_error("NPY::load(): failed read");
                out = 0;
                for (size_t b = 0; b < sizeof(T_); b++)
                    out |= buffer[b] << (b << 3);
            };

            // The first 6 bytes are a magic string: exactly \x93NUMPY.
            char buffer[6];
            if (iostrm_.read(buffer, 6).fail())
                throw std::runtime_error("NPY::load(): failed read");
            if (strncmp(magic_, buffer, 6) != 0)
                throw std::runtime_error("NPY::load(): magic does not match");

            // The next 1 byte is an unsigned byte: the major version number of the file format, e.g. \x01.
            // The next 1 byte is an unsigned byte: the minor version number of the file format, e.g. \x00. Note: the
            // version of the file format is not tied to the version of the numpy package.
            char major, minor;
            read_stream(iostrm_, major);
            read_stream(iostrm_, minor);
            if (!((major == 1 && minor == 0) || (major == 2 && minor == 0)))
                throw std::runtime_error("NPY::load(): unsupported npy version");

            // v1.0: The next 2 bytes form a little-endian unsigned short int: the length of the header data HEADER_LEN.
            // v2.0: The next 4 bytes form a little-endian unsigned int: the length of the header data HEADER_LEN.
            // The next HEADER_LEN bytes form the header data describing the array's format. It is an ASCII string which
            // contains a Python literal expression of a dictionary. It is terminated by a newline (\n) and padded with
            // spaces (\x20) to make the total of len(magic string) + 2 + len(length) + HEADER_LEN be evenly divisible
            // by 64 for alignment purposes.
            size_t headerlen;
            if (major == 1 && minor == 0) {
                uint16_t len;
                read_stream(iostrm_, len);
                headerlen = len;
                if ((6 + 2 + 2 + headerlen) & 63)
                    throw std::runtime_error("NPY::load(): unexpected header length");
            } else if (major == 2 && minor == 0) {
                uint32_t len;
                read_stream(iostrm_, len);
                headerlen = len;
                if ((6 + 2 + 4 + headerlen) & 63)
                    throw std::runtime_error("NPY::load(): unexpected header length");
            }

            char header[headerlen];
            if (iostrm_.read(header, std::streamsize(headerlen)).fail())
                throw std::runtime_error("NPY::load(): failed read");

            // The dictionary contains three keys:
            //
            // "descr" dtype.descr
            // An object that can be passed as an argument to the numpy.dtype constructor to create the array's dtype.
            //
            // "fortran_order" bool
            // Whether the array data is Fortran-contiguous or not. Since Fortran-contiguous arrays are a common form of
            // non-C-contiguity, we allow them to be written directly to disk for efficiency.
            //
            // "shape" tuple of int
            // The shape of the array.
            // For repeatability and readability, the dictionary keys are sorted in alphabetic order. This is for
            // convenience only. A writer SHOULD implement this if possible. A reader MUST NOT depend on this.
            auto parse_header = [](const std::string &header) {
                size_t d = header.find("'descr'");
                size_t ds = header.find('\'', d + 7);
                size_t de = header.find('\'', ds + 1);
                std::string descr = header.substr(ds + 1, de - ds - 1);

                size_t f = header.find("'fortran_order'");
                bool fortran_order = header[header.find_first_of("TF", f + 15)] == 'T';

                size_t s = header.find("'shape'");
                size_t ss = header.find('(', s + 7);
                size_t se = header.find(')', ss + 1);
                std::string shape_str = header.substr(ss + 1, se - ss - 1);
                if (!shape_str.empty() && shape_str.back() != ',')
                    shape_str += ',';
                size_t last = 0;
                std::vector<size_t> shape;
                while (true) {
                    size_t pos = shape_str.find(',', last);
                    if (pos == std::string::npos)
                        break;
                    shape.push_back(stoi(shape_str.substr(last, pos - last)));
                    last = pos + 1;
                }

                return make_tuple(descr, fortran_order, shape);
            };

            auto[descr, fortran_order, shape] = parse_header(std::string(header, headerlen));
            if (descr[1] != dtype<T>())
                throw std::runtime_error("NPY::load(): data type does not match");
            if (stoi(descr.substr(2)) != sizeof(T))
                throw std::runtime_error("NPY::load(): type size does not match");
            if (fortran_order) {
                fortran_order = false;
                reverse(shape.begin(), shape.end());
            }

            // Following the header comes the array data. If the dtype contains Python objects (i.e. dtype.hasobject is
            // True), then the data is a Python pickle of the array. Otherwise the data is the contiguous (either C- or
            // Fortran-, depending on fortran_order) bytes of the array. Consumers can figure out the number of bytes by
            // multiplying the number of elements given by the shape (noting that shape=() means there is 1 element) by
            // dtype.itemsize.
            ndarray<T> arr(shape);
            T *ptr = arr.data();
            size_t sz = arr.size() * sizeof(T);
            if (iostrm_.read((char *) ptr, std::streamsize(sz)).fail())
                throw std::runtime_error("NPY::load(): failed read");

            size_t tsz = typesize<T>();
            size_t htsz = tsz >> 1;
            if (descr[0] != endianness_() && htsz) {
                for (size_t i = 0; i < sz; i += tsz) {
                    char *ptr_l = (char *) ptr + i;
                    char *ptr_r = ptr_l + tsz - 1;
                    for (size_t b = 0; b < htsz; b++) {
                        using std::swap;
                        swap(*(ptr_l + b), *(ptr_r - b));
                    }
                }
            }

            return arr;
        }

        template<class NDArray>
        void save(const NDArray &arr, std::array<char, 2> version = {0, 0}) {
            static_assert(std::is_same<ndarray_impl<typename NDArray::value_type, typename NDArray::container_type>,
                    NDArray>());
            static_assert(dtype<typename NDArray::value_type>() != '?');

            if (mode_ && mode_ != 'w')
                throw std::runtime_error("NPY::save(): file not opened in 'w' mode");

            auto write_stream = []<typename T_>(std::iostream &iostrm, T_ out) {
                char buffer[sizeof(T_)];
                for (size_t b = 0; b < sizeof(T_); b++)
                    buffer[b] = out >> (b << 3) & 255;
                if (iostrm.write(buffer, sizeof(T_)).fail())
                    throw std::runtime_error("NPY::save(): failed write");
            };

            // The first 6 bytes are a magic string: exactly \x93NUMPY.
            if (iostrm_.write(magic_, 6).fail())
                throw std::runtime_error("NPY::save(): failed write");

            // The next 1 byte is an unsigned byte: the major version number of the file format, e.g. \x01.
            // The next 1 byte is an unsigned byte: the minor version number of the file format, e.g. \x00. Note: the
            // version of the file format is not tied to the version of the numpy package.
            // v1.0: The next 2 bytes form a little-endian unsigned short int: the length of the header data HEADER_LEN.
            // v2.0: The next 4 bytes form a little-endian unsigned int: the length of the header data HEADER_LEN.
            // The next HEADER_LEN bytes form the header data describing the array's format. It is an ASCII string which
            // contains a Python literal expression of a dictionary. It is terminated by a newline (\n) and padded with
            // spaces (\x20) to make the total of len(magic string) + 2 + len(length) + HEADER_LEN be evenly divisible
            // by 64 for alignment purposes.
            // The dictionary contains three keys:
            //
            // "descr" dtype.descr
            // An object that can be passed as an argument to the numpy.dtype constructor to create the array's dtype.
            //
            // "fortran_order" bool
            // Whether the array data is Fortran-contiguous or not. Since Fortran-contiguous arrays are a common form of
            // non-C-contiguity, we allow them to be written directly to disk for efficiency.
            //
            // "shape" tuple of int
            // The shape of the array.
            // For repeatability and readability, the dictionary keys are sorted in alphabetic order. This is for
            // convenience only. A writer SHOULD implement this if possible. A reader MUST NOT depend on this.
            auto make_header = [](const std::string &descr, bool fortran_order,
                                  const typename NDArray::container_type &shape) {
                std::string header;
                header += "{'descr': '";
                header += descr;
                header += "', 'fortran_order': ";
                header += fortran_order ? "True" : "False";
                header += ", 'shape': (";
                for (auto s : shape)
                    header += std::to_string(s) + ", ";
                if (shape.size() > 1)
                    header.resize(header.length() - 2);
                else if (shape.size() == 1)
                    header.resize(header.length() - 1);
                header += "), }";
                return header;
            };

            char major = version[0], minor = version[1];
            std::string header = make_header(std::string() + endianness_() + dtype<typename NDArray::value_type>()
                                             + std::to_string(sizeof(typename NDArray::value_type)), false,
                                             arr.shape());
            if (major == 0 && minor == 0) {
                size_t padding = 63 - ((6 + 2 + 2 + header.length()) & 63);
                if (header.length() + padding + 1 < 65536)
                    major = 1, minor = 0;
                else
                    major = 2, minor = 0;
            }
            size_t padding;
            if (major == 1 && minor == 0)
                padding = 63 - ((6 + 2 + 2 + header.length()) & 63);
            else if (major == 2 && minor == 0)
                padding = 63 - ((6 + 2 + 4 + header.length()) & 63);
            else
                throw std::runtime_error("NPY::save(): unsupported npy version");
            header.append(padding, ' ');
            header += '\n';

            write_stream(iostrm_, major);
            write_stream(iostrm_, minor);
            if (major == 1 && minor == 0) {
                if (header.length() >= 65536)
                    throw std::runtime_error("NPY::save(): header size too large for version 1.0");
                uint16_t len = header.length();
                write_stream(iostrm_, len);
            } else if (major == 2 && minor == 0) {
                uint32_t len = header.length();
                write_stream(iostrm_, len);
            }

            if (iostrm_.write(header.c_str(), std::streamsize(header.length())).fail())
                throw std::runtime_error("NPY::save(): failed write");

            // Following the header comes the array data. If the dtype contains Python objects (i.e. dtype.hasobject is
            // True), then the data is a Python pickle of the array. Otherwise the data is the contiguous (either C- or
            // Fortran-, depending on fortran_order) bytes of the array. Consumers can figure out the number of bytes by
            // multiplying the number of elements given by the shape (noting that shape=() means there is 1 element) by
            // dtype.itemsize.
            const typename NDArray::value_type *ptr = arr.data();
            size_t sz = arr.size() * sizeof(typename NDArray::value_type);
            if (iostrm_.write((char *) ptr, std::streamsize(sz)).fail())
                throw std::runtime_error("NPY::save(): failed write");
        }

        template<class T>
        constexpr static char dtype() { return '?'; }

        template<class T>
        constexpr static size_t typesize() {
            return sizeof(T);
        }

    private:
        static char endianness_() {
            union {
                uint16_t s;
                char c[2];
            } e = {0x3E3C};
            return e.c[0];
        }

        inline static char magic_[6] = {-109, 'N', 'U', 'M', 'P', 'Y'};

        char mode_;
        std::fstream fstrm_;
        std::iostream iostrm_;
    };

    template<>
    constexpr char NPY::dtype<bool>() { return 'b'; }

    template<>
    constexpr char NPY::dtype<char>() { return 'i'; }

    template<>
    constexpr char NPY::dtype<short>() { return 'i'; }

    template<>
    constexpr char NPY::dtype<int>() { return 'i'; }

    template<>
    constexpr char NPY::dtype<long>() { return 'i'; }

    template<>
    constexpr char NPY::dtype<long long>() { return 'i'; }

    template<>
    constexpr char NPY::dtype<unsigned char>() { return 'u'; }

    template<>
    constexpr char NPY::dtype<unsigned short>() { return 'u'; }

    template<>
    constexpr char NPY::dtype<unsigned int>() { return 'u'; }

    template<>
    constexpr char NPY::dtype<unsigned long>() { return 'u'; }

    template<>
    constexpr char NPY::dtype<unsigned long long>() { return 'u'; }

    template<>
    constexpr char NPY::dtype<float>() { return 'f'; }

    template<>
    constexpr char NPY::dtype<double>() { return 'f'; }

    template<>
    constexpr char NPY::dtype<long double>() { return 'f'; }

    template<>
    constexpr char NPY::dtype<std::complex<float>>() { return 'c'; }

    template<>
    constexpr char NPY::dtype<std::complex<double>>() { return 'c'; }

    template<>
    constexpr char NPY::dtype<std::complex<long double>>() { return 'c'; }

    template<>
    constexpr size_t NPY::typesize<std::complex<float>>() { return sizeof(float); }

    template<>
    constexpr size_t NPY::typesize<std::complex<double>>() { return sizeof(double); }

    template<>
    constexpr size_t NPY::typesize<std::complex<long double>>() { return sizeof(long double); }

}
