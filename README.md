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

(To be continued...)
