import matplotlib.pyplot as plt
import numpy as np

file_rd = open("raw_accel11_12_2021_191401.txt", "r")

lines = file_rd.readlines()

xg = []
yg = []
zg = []

xa = []
ya = []
za = []

file_rd.close()
for line in lines:
    res = line.split(' ')
    xg.append(float(res[0]))
    yg.append(float(res[1]))
    zg.append(float(res[2]))
    xa.append(float(res[3]))
    ya.append(float(res[4]))
    za.append(float(res[5]))
t = np.arange(len(xa))
plt.plot(t, za)
plt.show()