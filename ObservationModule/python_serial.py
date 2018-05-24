import serial
import time

#define ZEBROBUS_OBS_MASTER_READ 			0xF0
#define ZEBROBUS_OBS_MASTER_WRITE 			0x0F
#define VREGS_OBS_INFRAREDRAW_L 	0x50
#define VREGS_OBS_INFRAREDRAW_H 	0x51
#define VREGS_OBS_DISTANCERAW0		0x52
#define VREGS_OBS_DISTANCERAW1		0x53

ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=1)

time.sleep(2)

length = 13
register = 0x30
while True:

    # data_arr = [0xF0, 0x30, 13]
    data_arr = [0xF0, register, length]
    ser.write(bytearray(data_arr));

    data = ser.read(length)
    # print("%03d %03d %03d %03d" % (data[0], data[1], data[2], data[3]))
    print([int(n) for n in data])
    time.sleep(0.1)


