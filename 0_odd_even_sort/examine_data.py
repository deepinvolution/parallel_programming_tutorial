import sys
import numpy as np

N = int(sys.argv[1])
in_file = open(sys.argv[2], 'rb')
out_file = open(sys.argv[3], 'rb')

in_data = np.fromfile(in_file, dtype=np.int32) 
out_data = np.fromfile(out_file, dtype=np.int32)

print("Test result: ", np.all(np.sort(in_data) == out_data))