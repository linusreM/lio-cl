import keyboard

import serial

import time

import numpy as np

def rotate_point(x, y, degrees):
	rx = (x*np.cos(np.deg2rad(degrees))) - (y*np.sin(np.deg2rad(degrees)))
	ry = (y*np.cos(np.deg2rad(degrees))) + (x*np.sin(np.deg2rad(degrees)))
	return rx, ry


ser = serial.Serial("/dev/ttyACM0")

time.sleep(3)

ser.write(('k' + '\r').encode('ascii'))

time.sleep(1)

ser.write(('lcd-init' + '\r').encode('ascii'))
time.sleep(0.1)


color = 0
p1x = 0.0
p1y = 1.0
p2x = 0
p2y = 0
p3x = 0
p3y = 0

offset_x = 80
offset_y = 40

scale = 10.0
color_scale = 0.12


p2x, p2y = rotate_point(p1x, p1y, 360/3)
p3x, p3y = rotate_point(p2x, p2y, 360/3)



while True:
	ser.write(('lcd-tri {:x} {} {} {} {} {} {}'.format(color, int((p1x*scale)+offset_x), int((p1y*scale)+offset_y), int((p2x*scale)+offset_x), int((p2y*scale)+offset_y), int((p3x*scale)+offset_x), int((p3y*scale)+offset_y)) + '\r').encode('ascii'))
	time.sleep(0.016)
	#ser.write(('lcd-tri {} {} {} {} {} {} {}'.format("0000", int((p1x*scale)+offset_x), int((p1y*scale)+offset_y), int((p2x*scale)+offset_x), int((p2y*scale)+offset_y), int((p3x*scale)+offset_x), int((p3y*scale)+offset_y)) + '\r').encode('ascii'))
	#time.sleep(0.016)
	scale = (5 * (np.sin(time.time())+1.0)) + 10
	p1x, p1y = rotate_point(p1x, p1y, 0.4)
	p2x, p2y = rotate_point(p2x, p2y, 0.4)
	p3x, p3y = rotate_point(p3x, p3y, 0.4)
	offset_y = 40 + (35 * np.sin(time.time()*0.3))
	offset_x = 80 + (70 * np.sin(time.time()*0.47))
	color = ((64*32)*int(16.0 * (np.sin((time.time()*color_scale)+((2.0*np.pi)/3.0))+ 1.0))) + ((32)*int(32.0 * (np.sin((time.time()*color_scale)+((4.0*np.pi)/3.0)) + 1.0))) + int(16.0 * (np.sin(time.time()*color_scale) + 1.0))
	color %= 65535
	