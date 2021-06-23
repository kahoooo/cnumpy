#include <cassert>
#include <vector>
#include "cnumpy/ndarray.hpp"

using namespace std;
using namespace cnumpy;

int main() {
  // default constructor, empty array expected, destruct immediately
  {
    ndarray<int> arr;
    assert(arr.size() == 0);
  }

  // constructor with explicit shape array, 120 elements expected
  {
    vector<size_t> shape = {2, 3, 4, 5};
    ndarray<int> arr(shape);
    assert(arr.size() == 120);
    assert(arr.ndim() == 4);
  }

  // constructor with implicit shape array, 120 elements expected
  {
    ndarray<int> arr(2, 3, 4, 5);
    assert(arr.size() == 120);

    // indexing
    size_t cnt = 0;
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
    ndarray<int> arr1(2, 3, 4, 5);
    size_t cnt = 0;
    for (size_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < 3; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            arr1(i, j, k, l) = cnt++;
          }
        }
      }
    }

    ndarray<int> arr2(arr1);
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
    ndarray<int> arr(ndarray<int>(2, 3, 4, 5));
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
  }

  // copy-assignment operator
  {
    ndarray<int> arr1(2, 3, 4, 5);
    size_t cnt = 0;
    for (size_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < 3; j++) {
        for (size_t k = 0; k < 4; k++) {
          for (size_t l = 0; l < 5; l++) {
            arr1(i, j, k, l) = cnt++;
          }
        }
      }
    }

    ndarray<int> arr2 = arr1;
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
    ndarray<int> arr = ndarray<int>(2, 3, 4, 5);
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
  }

  return 0;
}
