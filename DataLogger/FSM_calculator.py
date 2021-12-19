import numpy as np
import struct

SAMPLE_RATE = 104 #Hz

def short_timer(miliseconds):
    max_time = (255 / SAMPLE_RATE) * 1000 # Maximum miliseconds
    if(miliseconds > max_time):
        return -1
    return hex(int((miliseconds / 1000) * SAMPLE_RATE))

def long_timer(miliseconds):
    max_time = (65535 / SAMPLE_RATE) * 1000 # Maximum miliseconds
    if(miliseconds > max_time):
        return -1
    return hex(int((miliseconds / 1000) * SAMPLE_RATE))

def thresh_converter(mg):
    return np.float16(mg).byteswap().tobytes().hex()

if __name__ == "__main__":
    print("Short timer samples: ", short_timer(2000))
    print("Long timer samples: ", long_timer(2500))
    print("Thresh mg: ", thresh_converter(-1.200))