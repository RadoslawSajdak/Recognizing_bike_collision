import smbus
from time import sleep
import numpy as np
from datetime import datetime

# File #
now = datetime.now()
current_time = now.strftime("%d_%m_%Y_%H%M%S")
print(current_time)
file_wr = open("raw_accel" + str(current_time)+".txt", "w")

# Accelerometer #
DEVICE_BUS = 1
DEVICE_ADDR = 0x6A
bus = smbus.SMBus(DEVICE_BUS)
bus.write_byte_data(DEVICE_ADDR, 0x10, 0x6C) #AccelOn
bus.write_byte_data(DEVICE_ADDR, 0x11, 0x62) #GyroOn
while(1):
    try:
        status = bus.read_byte_data(DEVICE_ADDR,0x1E) # Get status reg
        if(status & 0x03):
            raw = bus.read_i2c_block_data(DEVICE_ADDR, 0x22, 12)
            xg = np.int32(np.int16(((raw[1] << 8) | raw[0])) * 4.374)
            yg = np.int32(np.int16(((raw[3] << 8) | raw[2])) * 4.375)
            zg = np.int32(np.int16(((raw[5] << 8) | raw[4])) * 4.375)
            xa = np.int16(((raw[7] << 8) | raw[6])) * 0.244
            ya = np.int16(((raw[9] << 8) | raw[8])) * 0.244
            za = np.int16(((raw[11] << 8) | raw[10])) * 0.244
            print("%.04f %.04f %.04f" % (xg, yg, zg))
            file_wr.write("%.04f %.04f %.04f %.04f %.04f %.04f \n" % (xg, yg, zg, xa, ya, za))
        sleep(0.002)
    except KeyboardInterrupt:
        file_wr.close()
        break
print("Reading done!")