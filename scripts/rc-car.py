import keyboard

import serial

import time



accell = 500
decell = 250

mot_a = 0
mot_b = 0


up = 0
down = 0
right = 0
left = 0



ser = serial.Serial("/dev/ttyACM0")

time.sleep(1)

ser.write(('k' + '\r').encode('ascii'))

time.sleep(1)

ser.write(('mot-init A' + '\r').encode('ascii'))
time.sleep(0.1)

ser.write(('mot-init B' + '\r').encode('ascii'))
time.sleep(0.1)


while True:
	up = 0
	down = 0
	left = 0
	right = 0
	if(keyboard.is_pressed('w')):
		up = 1
	if(keyboard.is_pressed('a')):
		left = 1
	if(keyboard.is_pressed('s')):
		down = 1
	if(keyboard.is_pressed('d')):
		right = 1

	time.sleep(0.05)
	if(mot_a == 0):
		mot_a = 1
	mot_a += (accell*(left+up)) - (accell*(right+down)) - (decell*((mot_a/abs(mot_a))))
	if((mot_a) > 1023):
		mot_a = 1023
	if((mot_a) < -1023):
		mot_a = -1023
	ser.write(('mot-speed A {}'.format(int(mot_a)) + '\r').encode('ascii'))

	time.sleep(0.05)
	if(mot_b == 0):
		mot_b = 1
	mot_b += (accell*(right+up)) - (accell*(left+down)) - (decell*((mot_b/abs(mot_b))))
	if((mot_b) > 1023):
		mot_b = 1023
	if((mot_b) < -1023):
		mot_b = -1023
	ser.write(('mot-speed B {}'.format(int(mot_b)) + '\r').encode('ascii'))


	print("mot-speed B {:d}".format(int(mot_b)).encode())
	print(mot_a, mot_b)
