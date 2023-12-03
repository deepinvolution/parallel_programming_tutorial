import sys
import numpy as np

N = int(sys.argv[1])
in_file = sys.argv[2]
# print("Num data = ", N)
# print("In file = ", in_file)
data = np.random.randint(low=0, high=10, size=N)
with open(in_file, 'wb') as file:
    for x in data:
        file.write(int(x).to_bytes(4, byteorder='little', signed=True))