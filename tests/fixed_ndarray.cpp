#include <cassert>
#include "cnumpy/ndarray.hpp"

using namespace std;
using namespace cnumpy;

int main() {
    // default constructor, empty arrays expected if ndim > 0, one element expected if ndim == 0
    {
        ndarray<int, 0> arr0;
        assert(arr0.size() == 1);
        assert(&arr0() == arr0.data());

        ndarray<int, 1> arr1;
        assert(arr1.size() == 0);
        ndarray<int, 2> arr2;
        assert(arr2.size() == 0);
    }

    // constructor with explicit shape array, 120 elements expected
    {
        array<size_t, 4> shape = {2, 3, 4, 5};
        ndarray<int, 4> arr(shape);
        assert(arr.size() == 120);
        assert(arr.ndim() == 4);
    }

    // constructor with variadic arguments, 120 elements expected
    {
        ndarray<int, 4> arr(2, 3, 4, 5);
        assert(arr.size() == 120);

        // indexing
        int cnt = 0;
        assert((&arr[{}] == &arr.data()[cnt]));
        for (size_t i = 0; i < 2; i++) {
            assert((&arr[{i}] == &arr.data()[cnt]));
            for (size_t j = 0; j < 3; j++) {
                assert((&arr[{i, j}] == &arr.data()[cnt]));
                for (size_t k = 0; k < 4; k++) {
                    assert((&arr[{i, j, k}] == &arr.data()[cnt]));
                    for (size_t l = 0; l < 5; l++) {
                        assert((&arr[{i, j, k, l}] == &arr.data()[cnt]));
                        cnt++;
                    }
                }
            }
        }

        // indexing with variadic arguments
        cnt = 0;
        assert(&arr() == &arr.data()[cnt]);
        for (size_t i = 0; i < 2; i++) {
            assert(&arr(i) == &arr.data()[cnt]);
            for (size_t j = 0; j < 3; j++) {
                assert(&arr(i, j) == &arr.data()[cnt]);
                for (size_t k = 0; k < 4; k++) {
                    assert(&arr(i, j, k) == &arr.data()[cnt]);
                    for (size_t l = 0; l < 5; l++) {
                        assert(&arr(i, j, k, l) == &arr.data()[cnt]);
                        cnt++;
                    }
                }
            }
        }
    }

    // copy constructor
    {
        ndarray<int, 4> arr1(2, 3, 4, 5);
        int cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        arr1(i, j, k, l) = cnt++;
                    }
                }
            }
        }

        ndarray<int, 4> arr2(arr1);
        cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        assert(arr1(i, j, k, l) == cnt);
                        arr1[{i, j, k, l}] = 0;
                        assert(arr2(i, j, k, l) == cnt);
                        cnt++;
                    }
                }
            }
        }
    }

    // move constructor
    {
        ndarray<int, 4> arr(ndarray<int, 4>(2, 3, 4, 5));
        int cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        arr(i, j, k, l) = cnt++;
                    }
                }
            }
        }
    }

    // copy-assignment operator
    {
        ndarray<int, 4> arr1(2, 3, 4, 5);
        int cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        arr1(i, j, k, l) = cnt++;
                    }
                }
            }
        }

        ndarray<int, 4> arr2 = arr1;
        cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        assert(arr1(i, j, k, l) == cnt);
                        arr1[{i, j, k, l}] = 0;
                        assert(arr2(i, j, k, l) == cnt);
                        cnt++;
                    }
                }
            }
        }
    }

    // move-assignment operator
    {
        ndarray<int, 4> arr = ndarray<int, 4>(2, 3, 4, 5);
        int cnt = 0;
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 4; k++) {
                    for (size_t l = 0; l < 5; l++) {
                        arr(i, j, k, l) = cnt++;
                    }
                }
            }
        }
    }

    return 0;
}
