cmake_minimum_required(VERSION 3.1)
project(CNUMPY)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

install(DIRECTORY include/cnumpy DESTINATION include)

add_executable(test_fixed_ndarray tests/fixed_ndarray.cpp)
target_include_directories(test_fixed_ndarray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME test_fixed_ndarray COMMAND test_fixed_ndarray)

add_executable(test_variable_ndarray tests/variable_ndarray.cpp)
target_include_directories(test_variable_ndarray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME test_variable_ndarray COMMAND test_variable_ndarray)

add_executable(test_ndarray_misc tests/ndarray_misc.cpp)
target_include_directories(test_ndarray_misc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME test_ndarray_misc COMMAND test_ndarray_misc)
