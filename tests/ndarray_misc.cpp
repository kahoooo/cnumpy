#include <cassert>
#include <vector>
#include "cnumpy/ndarray.hpp"

using namespace std;
using namespace cnumpy;

int main() {
  // reshape
  {
    ndarray<int, 4> arr(2, 3, 4, 5);
    size_t cnt = 0;
    for (size_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < 3; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            arr(i, j, k, l) = cnt++;
          }
        }
      }
    }
    arr.reshape(1, 6, 4, 5);
    cnt = 0;
    for (size_t i = 0; i < 1; i++) {
      for (size_t j = 0; j < 6; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            assert(arr(i, j, k, l) == cnt);
            cnt++;
          }
        }
      }
    }
  }

  {
    ndarray<int> arr(2, 3, 4, 5);
    size_t cnt = 0;
    for (size_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < 3; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            arr(i, j, k, l) = cnt++;
          }
        }
      }
    }
    arr.reshape(6, 4, 5);
    cnt = 0;
    for (size_t i = 0; i < 1; i++) {
      for (size_t j = 0; j < 6; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            assert(arr(j, k, l) == cnt);
            cnt++;
          }
        }
      }
    }
  }

  // shared ndarray
  {
    ndarray<int, 4> arr1(2, 3, 4, 5);
    auto arr2 = arr1.make_shared(array<size_t, 3>{6, 4, 5});
    auto arr3 = arr1.make_shared<ndarray<int, 3>>(6, 4, 5);
    auto arr4 = arr1.make_shared(vector<size_t>{6, 4, 5});
    auto arr5 = arr1.make_shared<ndarray<int>>(6, 4, 5);
    assert(arr1.data() == arr2.data());
    assert(arr1.data() == arr3.data());
    assert(arr1.data() == arr4.data());
    assert(arr1.data() == arr5.data());
    for (size_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < 3; j++) {
        size_t ij = i * 3 + j;
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            assert(&arr1(i, j, k, l) == &arr2(ij, k, l));
            assert(&arr1(i, j, k, l) == &arr3(ij, k, l));
            assert(&arr1(i, j, k, l) == &arr4(ij, k, l));
            assert(&arr1(i, j, k, l) == &arr5(ij, k, l));
          }
        }
      }
    }
  }

  return 0;
}
