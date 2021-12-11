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
while(1):
    try:
        status = bus.read_byte_data(DEVICE_ADDR,0x1E) # Get status reg
        if(status & 0x01):
            raw = bus.read_i2c_block_data(DEVICE_ADDR, 0x28, 6)
            x = np.int16(((raw[1] << 8) | raw[0])) * 0.244
            y = np.int16(((raw[3] << 8) | raw[2])) * 0.244
            z = np.int16(((raw[5] << 8) | raw[4])) * 0.244
#           print("%.04f %.04f %.04f" % (x, y, z))
            file_wr.write("%.04f %.04f %.04f \n" % (x, y, z))
        sleep(0.002)
    except KeyboardInterrupt:
        file_wr.close()
        break
print("Reading done!")
