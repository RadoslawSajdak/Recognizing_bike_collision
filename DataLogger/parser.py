import matplotlib.pyplot as plt
import numpy as np

file_rd = open("raw_accel11_12_2021_183119.txt", "r")

lines = file_rd.readlines()

x = []
y = []
z = []

file_rd.close()
for line in lines:
    res = line.split(' ')
    x.append(float(res[0]))
    y.append(float(res[1]))
    z.append(float(res[2]))
t = np.arange(len(x))
plt.plot(t, z)
plt.show()