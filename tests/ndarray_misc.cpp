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
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 4; k++) {
          for (int l = 0; l < 5; l++) {
            arr(i, j, k, l) = cnt++;
          }
        }
      }
    }
    arr.reshape(1, 6, 4, 5);
    cnt = 0;
    for (int i = 0; i < 1; i++) {
      for (int j = 0; j < 6; j++) {
        for (int k = 0; k < 4; k++) {
          for (int l = 0; l < 5; l++) {
            assert(arr(i, j, k, l) == cnt);
            cnt++;
          }
        }
      }
    }
  }

  return 0;
}
