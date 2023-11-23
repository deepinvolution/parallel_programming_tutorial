import numpy as np
data = np.random.randint(low=0, high=10, size=40)
with open('./build/data.bin', 'wb') as file:
    for x in data:
        file.write(int(x).to_bytes(4, byteorder='little', signed=True))