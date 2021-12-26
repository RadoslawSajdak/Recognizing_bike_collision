import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, lfilter, freqz

file_rd = open("raw_accel13_12_2021_072106.txt", "r")

lines = file_rd.readlines()

CUT_FRONT = 50

xg = []
yg = []
zg = []

xa = []
ya = []
za = []
raw_data = [ [], [], [], [], [], []]

order = 6
fs = 416.0
cutoff = 4

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

b, a = butter_lowpass(cutoff, fs, order)
file_rd.close()

#############################################
#               Data Analysis               #
#############################################

for line in lines:
    res = line.split(' ')
    raw_data[0].append(float(res[2])/1000)  # Gyro X
    raw_data[1].append(float(res[1])/1000)  # Gyro Y
    raw_data[2].append(float(res[0])/1000)  # Gyro Z
    raw_data[3].append(float(res[5])/-1000)  # Accel X
    raw_data[4].append(float(res[4])/1000)  # Accel Y
    raw_data[5].append(float(res[3])/1000)  # Accel Z

filtered_data = []
titles = ["Gyroscope X", "Gyroscope Y", "Gyroscope Z", "Accelerometer X", "Accelerometer Y", "Accelerometer Z"]
y_labels = ["dps", "dps", "dps", "g[m/s^2]", "g[m/s^2]", "g[m/s^2]"]
x_labels = ["t[s]", "t[s]", "t[s]", "t[s]", "t[s]", "t[s]"] 
plt.figure(figsize=(10,7))
plt.suptitle("Upadek ze stromego zbocza")
for axis, n, tit, y_l, x_l in zip(raw_data[3:], range(len(raw_data[3:])), titles[3:], y_labels[3:], x_labels[3:]):
    filtered_data.append(butter_lowpass_filter(axis[CUT_FRONT:], cutoff, fs, order))
    t = np.arange(len(axis[CUT_FRONT:]))/416
    plt.subplot(3,1,n + 1)
    plt.plot(t, axis[CUT_FRONT:])
    plt.ylabel(y_l)
    plt.xlabel(x_l)
    sec = plt.twinx()
    sec.plot(t,filtered_data[n],color='red')
    plt.tight_layout()
    plt.title(tit)
    
plt.show()