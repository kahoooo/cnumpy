#pragma once

#include <algorithm>    // copy, swap
#include <array>
#include <functional>   // multiplies
#include <limits>       // numeric_limits
#include <numeric>      // accumulate, exclusive_scan
#include <type_traits>  // conditional_t
#include <vector>

namespace cnumpy {

  template<class T, class C = std::vector<size_t>>
  class ndarray_impl {
  private:
    size_t size_;
    T *data_;
    C shape_, strides_;

  public:
    // default constructor
    ndarray_impl() : size_(0), data_(nullptr), shape_{}, strides_{} {};

    // copy constructor
    ndarray_impl(const ndarray_impl<T, C> &arr) :
    size_(arr.size_), data_(new T[size_]), shape_(arr.shape_), strides_(arr.strides_) {
      std::copy(arr.data_, arr.data_+size_, data_);
    };

    // move constructor
    ndarray_impl(ndarray_impl<T, C> &&arr) : ndarray_impl() {
      swap(*this, arr);
    }

    // copy-assignment operator
    ndarray_impl &operator=(ndarray_impl<T, C> arr) {
      swap(*this, arr);
      return *this;
    }

    // copy-assignment operator
    ndarray_impl &operator=(ndarray_impl<T, C> &&arr) {
      swap(*this, arr);
      return *this;
    }

    // destructor
    ~ndarray_impl() {
      delete[] data_;
    }

    ndarray_impl(const C &shape) :
    size_(std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>())),
    data_(new T[size_]), shape_(shape), strides_(shape) {
      std::exclusive_scan(shape.rbegin(), shape.rend(), strides_.rbegin(), 1, std::multiplies<size_t>());
    }

    template<typename... Ints>
    ndarray_impl(Ints... ints) : ndarray_impl(C{size_t(ints)...}) {
      static_assert((std::is_integral<Ints>() && ...));
    }

    const T* data() const noexcept { return data_; }
    T* data() noexcept { return data_; }

    size_t size() const noexcept { return size_; }
    size_t ndim() const noexcept { return shape_.size(); }
    const C &shape() const noexcept { return shape_; }
    const C &strides() const noexcept { return strides_; }

    // indexing
    const T &operator[](const C &ndindex) const {
      size_t idx = 0;
      for (size_t i = 0; i < ndindex.size(); i++)
        idx += strides_[i] * ndindex[i];
      return data_[idx];
    }

    T &operator[](const C &ndindex) {
      return const_cast<T &>(static_cast<const ndarray_impl<T, C> &>(*this)[ndindex]);
    }

    template<typename... Ints>
    const T &operator()(Ints... ints) const {
      static_assert((std::is_integral<Ints>() && ...));
      return operator[](C{size_t(ints)...});
    }

    template<typename... Ints>
    T &operator()(Ints... ints) {
      return const_cast<T &>(static_cast<const ndarray_impl<T, C> &>(*this)(ints...));
    }

    friend void swap(ndarray_impl<T, C> &first, ndarray_impl<T, C> &second) {
      using std::swap;
      swap(first.size_, second.size_);
      swap(first.data_, second.data_);
      swap(first.shape_, second.shape_);
      swap(first.strides_, second.strides_);
    }
  };

  template<class T, size_t N = size_t(-1)>
  using ndarray = ndarray_impl<T, std::conditional_t<N == -1, std::vector<size_t>, std::array<size_t, N>>>;

}
