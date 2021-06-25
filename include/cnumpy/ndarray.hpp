#pragma once

#include <algorithm>    // copy, swap
#include <array>
#include <functional>   // multiplies
#include <memory>       // shared_ptr
#include <numeric>      // accumulate, exclusive_scan
#include <stdexcept>    // runtime_error
#include <type_traits>  // conditional_t, is_integral, is_same
#include <vector>

namespace cnumpy {

  template<class T, class Container = std::vector<size_t>>
  class ndarray_impl {
    template<class T_, class Container_>
    friend class ndarray_impl;

  public:
    using value_type = T;
    using container_type = Container;

    // default constructor
    ndarray_impl() : size_(0), shape_{}, strides_{}, data_(nullptr), shared_data_(data_) {};

    // copy constructor
    ndarray_impl(const ndarray_impl<value_type, container_type> &arr) :
    size_(arr.size_), shape_(arr.shape_), strides_(arr.strides_), data_(new value_type[size_]), shared_data_(data_) {
      std::copy(arr.data_, arr.data_+size_, data_);
    };

    // move constructor
    ndarray_impl(ndarray_impl<value_type, container_type> &&arr) : ndarray_impl() {
      swap(*this, arr);
    }

    // copy-assignment operator
    ndarray_impl &operator=(ndarray_impl<value_type, container_type> arr) {
      swap(*this, arr);
      return *this;
    }

    // move-assignment operator
    ndarray_impl &operator=(ndarray_impl<value_type, container_type> &&arr) {
      swap(*this, arr);
      return *this;
    }

    // destructor
    ~ndarray_impl() {}

    ndarray_impl(const container_type &shape) :
    size_(std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>())),
    shape_(shape), strides_(shape), data_(new value_type[size_]), shared_data_(data_) {
      std::exclusive_scan(shape.rbegin(), shape.rend(), strides_.rbegin(), 1, std::multiplies<size_t>());
    }

    template<typename... Ints>
    ndarray_impl(Ints... ints) : ndarray_impl(container_type{size_t(ints)...}) {
      static_assert((std::is_integral<Ints>() && ...));
    }

    const value_type* data() const noexcept { return data_; }
    value_type* data() noexcept { return data_; }

    size_t size() const noexcept { return size_; }
    size_t ndim() const noexcept { return shape_.size(); }
    const container_type &shape() const noexcept { return shape_; }
    const container_type &strides() const noexcept { return strides_; }

    void reshape(const container_type &shape) {
      size_t size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>());
      if (size != size_)
        throw std::runtime_error("ndarray_impl<T, Container>::reshape(): sizes do not match");
      shape_ = shape;
      std::exclusive_scan(shape.rbegin(), shape.rend(), strides_.rbegin(), 1, std::multiplies<size_t>());
    }

    template<typename... Ints>
    void reshape(Ints... ints) {
      static_assert((std::is_integral<Ints>() && ...));
      reshape(container_type{size_t(ints)...});
    }

    // indexing
    const value_type &operator[](const container_type &ndindex) const {
      size_t idx = 0;
      for (size_t i = 0; i < ndindex.size(); i++)
        idx += strides_[i] * ndindex[i];
      return data_[idx];
    }

    value_type &operator[](const container_type &ndindex) {
      return const_cast<value_type &>(static_cast<const ndarray_impl<value_type, container_type> &>(*this)[ndindex]);
    }

    template<typename... Ints>
    const value_type &operator()(Ints... ints) const {
      static_assert((std::is_integral<Ints>() && ...));
      return operator[](container_type{size_t(ints)...});
    }

    template<typename... Ints>
    value_type &operator()(Ints... ints) {
      return const_cast<value_type &>(static_cast<const ndarray_impl<value_type, container_type> &>(*this)(ints...));
    }

    // utilities
    template<class Container_>
    ndarray_impl<value_type, Container_> make_shared(const Container_ &shape) {
      size_t size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>());
      if (size != size_)
        throw std::runtime_error("ndarray_impl<T, Container>::make_shared(): sizes do not match");

      ndarray_impl<value_type, Container_> arr;
      arr.size_ = size;
      arr.shape_ = shape;
      arr.strides_ = shape;
      std::exclusive_scan(shape.rbegin(), shape.rend(), arr.strides_.rbegin(), 1, std::multiplies<size_t>());

      arr.data_ = data_;
      arr.shared_data_ = shared_data_;

      return arr;
    }

    template<class NDArray, typename... Ints>
    NDArray make_shared(Ints... ints) {
      static_assert(std::is_same<ndarray_impl<typename NDArray::value_type, typename NDArray::container_type>, NDArray>());
      static_assert(std::is_same<T, typename NDArray::value_type>());
      static_assert((std::is_integral<Ints>() && ...));
      return make_shared(typename NDArray::container_type{size_t(ints)...});
    }

    friend void swap(ndarray_impl<value_type, container_type> &first, ndarray_impl<value_type, container_type> &second) {
      using std::swap;
      swap(first.size_, second.size_);
      swap(first.shape_, second.shape_);
      swap(first.strides_, second.strides_);
      swap(first.data_, second.data_);
      swap(first.shared_data_, second.shared_data_);
    }

  private:
    size_t size_;
    container_type shape_, strides_;

    value_type *data_;
    std::shared_ptr<value_type[]> shared_data_;
  };

  template<class T, size_t N = size_t(-1)>
  using ndarray = ndarray_impl<T, std::conditional_t<N == -1, std::vector<size_t>, std::array<size_t, N>>>;

}
