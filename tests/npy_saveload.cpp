#include <cassert>
#include "cnumpy/ndarray.hpp"
#include "cnumpy/npy.hpp"

using namespace std;
using namespace cnumpy;

int main() {
    // int v1.0 and v2.0
    {
        ndarray<int, 3> arr(10, 11, 12);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    arr(i, j, k) = cnt++;
                }
            }
        }
        NPY npy("version_1_0_int.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("version_1_0_int.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    assert(cnt == arr(i, j, k));
                    cnt++;
                }
            }
        }
    }

    {
        ndarray<int, 3> arr(10, 11, 12);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    arr(i, j, k) = cnt++;
                }
            }
        }
        NPY npy("version_2_0_int.npy", 'w');
        npy.save(arr, {2, 0});
        npy.close();
    }

    {
        NPY npy("version_2_0_int.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    assert(cnt == arr(i, j, k));
                    cnt++;
                }
            }
        }
    }

    // double v1.0 and v2.0
    {
        ndarray<double, 3> arr(10, 11, 12);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    arr(i, j, k) = cnt++;
                }
            }
        }
        NPY npy("version_1_0_double.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("version_1_0_double.npy", 'r');
        auto arr = npy.load<double>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    assert(double(cnt) == arr(i, j, k));
                    cnt++;
                }
            }
        }
    }

    {
        ndarray<double, 3> arr(10, 11, 12);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    arr(i, j, k) = cnt++;
                }
            }
        }
        NPY npy("version_2_0_double.npy", 'w');
        npy.save(arr, {2, 0});
        npy.close();
    }

    {
        NPY npy("version_2_0_double.npy", 'r');
        auto arr = npy.load<double>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                for (int k = 0; k < arr.shape()[2]; k++) {
                    assert(double(cnt) == arr(i, j, k));
                    cnt++;
                }
            }
        }
    }

    // 2-dimension fixed and variable
    {
        ndarray<int, 2> arr(10, 11);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                arr(i, j) = cnt++;
            }
        }
        NPY npy("two_dimension_fixed.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("two_dimension_fixed.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                assert(cnt == arr(i, j));
                cnt++;
            }
        }
    }

    {
        ndarray<int> arr(10, 11);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                arr(i, j) = cnt++;
            }
        }
        NPY npy("two_dimension_variable.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("two_dimension_variable.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            for (int j = 0; j < arr.shape()[1]; j++) {
                assert(cnt == arr(i, j));
                cnt++;
            }
        }
    }

    // 1-dimension fixed and variable
    {
        ndarray<int, 1> arr(10);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            arr(i) = cnt++;
        }
        NPY npy("one_dimension_fixed.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("one_dimension_fixed.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            assert(cnt == arr(i));
            cnt++;
        }
    }

    {
        ndarray<int> arr(10);
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            arr(i) = cnt++;
        }
        NPY npy("one_dimension_variable.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("one_dimension_variable.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        int cnt = 0;
        for (int i = 0; i < arr.shape()[0]; i++) {
            assert(cnt == arr(i));
            cnt++;
        }
    }

    // 0-dimension fixed and variable
    {
        ndarray<int, 0> arr;
        arr() = 0;
        NPY npy("zero_dimension_fixed.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("zero_dimension_fixed.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        assert(arr() == 0);
    }

    {
        ndarray<int> arr;
        arr() = 0;
        NPY npy("zero_dimension_variable.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("zero_dimension_variable.npy", 'r');
        auto arr = npy.load<int>();
        npy.close();
        assert(arr() == 0);
    }

    return 0;
}
