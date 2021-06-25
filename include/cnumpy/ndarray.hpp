#pragma once

#include <algorithm>    // copy, swap
#include <array>
#include <functional>   // multiplies
#include <limits>       // numeric_limits
#include <numeric>      // accumulate, exclusive_scan
#include <type_traits>  // conditional_t
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
    ndarray_impl() : size_(0), data_(nullptr), shape_{}, strides_{} {};

    // copy constructor
    ndarray_impl(const ndarray_impl<value_type, container_type> &arr) :
    size_(arr.size_), data_(new value_type[size_]), shape_(arr.shape_), strides_(arr.strides_) {
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
    ~ndarray_impl() {
      delete[] data_;
    }

    ndarray_impl(const container_type &shape) :
    size_(std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>())),
    data_(new value_type[size_]), shape_(shape), strides_(shape) {
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

    friend void swap(ndarray_impl<value_type, container_type> &first, ndarray_impl<value_type, container_type> &second) {
      using std::swap;
      swap(first.size_, second.size_);
      swap(first.data_, second.data_);
      swap(first.shape_, second.shape_);
      swap(first.strides_, second.strides_);
    }

  private:
    size_t size_;
    value_type *data_;
    container_type shape_, strides_;
  };

  template<class T, size_t N = size_t(-1)>
  using ndarray = ndarray_impl<T, std::conditional_t<N == -1, std::vector<size_t>, std::array<size_t, N>>>;

}
