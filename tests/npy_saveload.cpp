#include <cassert>
#include "cnumpy/ndarray.hpp"
#include "cnumpy/npy.hpp"

using namespace std;
using namespace cnumpy;

int main() {
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
        NPY npy("version_1_0.npy", 'w');
        npy.save(arr);
        npy.close();
    }

    {
        NPY npy("version_1_0.npy", 'r');
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
        NPY npy("version_2_0.npy", 'w');
        npy.save(arr, {2, 0});
        npy.close();
    }

    {
        NPY npy("version_2_0.npy", 'r');
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

    return 0;
}
