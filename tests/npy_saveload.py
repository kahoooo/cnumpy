import os
import sys
import numpy as np


for f in ['version_1_0_int.npy', 'version_2_0_int.npy',
          'version_1_0_double.npy', 'version_2_0_double.npy',
          'two_dimension_fixed.npy', 'two_dimension_variable.npy',
          'one_dimension_fixed.npy', 'one_dimension_variable.npy',
          'zero_dimension_fixed.npy', 'zero_dimension_variable.npy']:
    out = np.load(os.path.join(sys.argv[1], f))
    assert np.all(np.arange(out.size) == out.flat)
