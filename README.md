# CNumpy

A header-only C++ library of multidimensional arrays and Numpy's npy/npz file reader/writer without dependency on external libraries.


## Motivation

1. C++'s support to multidimensional arrays has been limited. Two types of multidimensional arrays are `T[]...[]` and `vector<...vector<T>...>` while each of them has their own pitfalls; the former cannot be passed into functions without losing the shape information; the latter is not contiguous and is often slower.

2. While C++ is fast, analyzing and visualizing data in C++ are impractical so analysis are usually carried out in Python. C++ software need to output data in binary format, which are then read into Python for analysis. However, the implementation can be painful and it requires their own specification. In most of the case, the outputs are not compatible between each software. On the other hand, Numpy's npy/npz formats are already well-adopted due to the popularity of Python.

3. Linking and deploying libraries can be painful sometimes. Header-only libraries without dependency can be integrated into software by simply including the header files.

## Goals

This library aims to:

1. provide high-performance multidimensional arrays;

2. provide a user-friendly interface for reading/writing multidimensional arrays in npy/npz formats;

3. keep all implementation in header files to avoid compiling and linking libraries.


## Usage

All classes and functions are declared and implemented in the namespace `cnumpy`. In the following, it assumes the namespace is used, i.e., `using namespace cnumpy`.

### Multidimensional arrays

There are two types of multidimensional arrays: fixed- and variable-dimensional arrays (hereafter, f- and v-arrays). F-arrays are multidimensional arrays that have fixed numbers of dimensions while v-arrays have variable numbers of dimensions. Same interfaces are provided for both f- and v-arrays but have different restrictions. In general, f-arrays are superior to v-arrays in terms of performance if the numbers of dimensions are known while v-arrays are more flexible.


Defined in `cnumpy/ndarray.hpp`:
```c++
template<class T, class Container = std::vector<size_t>>
class ndarray_impl;

template<class T, size_t N = size_t(-1)>
using ndarray = ndarray_impl<T, std::conditional_t<N == -1, std::vector<size_t>, std::array<size_t, N>>>;
```

| Member types   | Definition |
| -------------- | ---------- |
| value_type     | T          |
| container_type | Container  |

| Member functions                                                             | Description                                                                    |
| ---------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| `const value_type* data() const noexcept;`                                   | Returns a direct pointer to the memory array used internally                   |
| `value_type* data();`                                                        | Same as above                                                                  |
| `size_t size() const noexcept;`                                              | Returns the number of elements in the array                                    |
| `size_t ndim() const noexcept;`                                              | Returns the number of array dimensions                                         |
| `const container_type &shape() const noexcept;`                              | Returns the array dimensions                                                   |
| `const container_type &strides() const noexcept;`                            | Returns number of elements to step in each dimension when traversing the array |
| `void reshape(const container_type &shape);`                                 | Modify the array dimensions                                                    |
| `void reshape(Ints... ints);`                                                | Same as above                                                                  |
| `const value_type &operator[](const container_type &ndindex) const;`         | Returns a reference to the element at the position in the array                |
| `value_type &operator[](const container_type &ndindex);`                     | Same as above                                                                  |
| `const value_type &operator()(Ints... ints) const;`                          | Same as above                                                                  |
| `value_type &operator()(Ints... ints);`                                      | Same as above                                                                  |
| `ndarray_impl<value_type, Container_> make_shared(const Container_ &shape);` | Returns a copy of shared array                                                 |
| `NDArray make_shared<NDArray>(Ints... ints);`                                | Same as above                                                                  |

| Non-member functions                                                                                   | Description                                |
| ------------------------------------------------------------------------------------------------------ | ------------------------------------------ |
| `void swap(ndarray_impl<value_type, container_type> &x, ndarray_impl<value_type, container_type> &y);` | Exchange the contents of x with those of y |

* `T`: type of elements
* `Container`: container used to store dimension information (`array<size_t, N>` for f-arrays, `vector<size_t>` for v-arrays)
* `N`: number of dimension (`SIZE_MAX` for v-arrays)
* `Ints`: any integral type


* `NDArray`: type of returned array
* `Container_`: container used by the returned array

#### Construction

F-arrays with datatype `T` and number of dimensions `N` can be declared with `ndarray<T, N>`. For v-arrays, simply drop the dimensions `ndarray<T>`. The default constructor constructs empty arrays. To construct an array with a certain size, pass the dimensions as an `array<size_t, N>` for f-arrays or as a `vector<size_t>` for v-arrays to the constructor. Alternatively, pass the unpacked dimensions as arguments to the constructor. For example,
```c++
// to construct an empty 2D array
ndarray<int, 2> empty_array;

// to construct a 3D array with shape (2, 3, 4)
std::array<size_t, 3> shape_arr = {2, 3, 4};
ndarray<int, 3> three_d_farray(shape_arr);
// alternatively, pass the unpacked dimensions as arguments
ndarray<int, 3> another_three_d_farray(2, 3, 4);

// to construct a 4D array with shape (2, 3, 4, 5) whose dimensions can be changed
std::vector<size_t> shape_vec = {2, 3, 4, 5};
ndarray<int> four_d_varray(shape_vec);
// alternatively, pass the unpacked dimensions as arguments
ndarray<int> another_four_d_varray(2, 3, 4, 5);
```

Copy and move constructor, as well as copy- and move-assignment operators are supported.
```c++
// copy constructor
ndarray<int, 3> three_d_farray_copy(three_d_farray);
// copy-assignment operator
ndarray<int, 3> another_three_d_farray_copy = another_three_d_farray;
// move constructor
ndarray<int, 3> three_d_farray_moved(ndarray<int, 3>{2, 3, 4});
// move-assignment operator
ndarray<int, 3> another_three_d_farray_moved = ndarray<int, 3>(2, 3, 4);
```

#### Indexing

Indexing can be achieved by providing indices as an `array<size_t, N>` for f-arrays or a `vector<size_t>` for v-arrays to `operator[]`. Alternatively, pass the indices as separated arguements to `operator()`. Unspecified indices are implicitly set to zeros.
```c++
&three_d_farray[{i, j, 0, 0}] == &three_d_farray[{i, j}];   // returns true;
&three_d_farray(i, j, k, 0) == &three_d_farray[{i, j, k}];  // returns true;
```

#### Reshaping, Slicing, Shared Memory...

(To be continued...)
