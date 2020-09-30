
import keyboard

import serial

import time

import numpy as np


ser = serial.Serial("/dev/ttyACM0")

time.sleep(1)

ser.write(('k' + '\r').encode('ascii'))

time.sleep(0.2)
for i in range(5000):
	ser.write(('audio-init' + '\r').encode('ascii'))
	time.sleep(0.01)


	ser.write(('audio-tone 400 440' + '\r').encode('ascii'))
	time.sleep(0.5)

	ser.write(('audio-tone 400 440' + '\r').encode('ascii'))
	time.sleep(0.5)

	ser.write(('audio-tone 400 440' + '\r').encode('ascii'))
	time.sleep(0.5)


	ser.write(('audio-tone 300 349' + '\r').encode('ascii'))
	time.sleep(0.35)

	#beep(cH, 150);
	ser.write(('audio-tone 100 523' + '\r').encode('ascii'))
	time.sleep(0.15)

	#beep(a, 500);
	ser.write(('audio-tone 400 440' + '\r').encode('ascii'))
	time.sleep(0.5)

	#beep(f, 350);
	ser.write(('audio-tone 300 349' + '\r').encode('ascii'))
	time.sleep(0.35)

	#beep(cH, 150);
	ser.write(('audio-tone 100 523' + '\r').encode('ascii'))
	time.sleep(0.15)

	#beep(a, 650);
	ser.write(('audio-tone 600 440' + '\r').encode('ascii'))
	time.sleep(0.65)

	time.sleep(0.30)

	#   beep(eH, 500);
	ser.write(('audio-tone 450 659' + '\r').encode('ascii'))
	time.sleep(0.50)
	#   beep(eH, 500);
	ser.write(('audio-tone 450 659' + '\r').encode('ascii'))
	time.sleep(0.50)
	#   beep(eH, 500);
	ser.write(('audio-tone 450 659' + '\r').encode('ascii'))
	time.sleep(0.50)
	#   beep(fH, 350);
	ser.write(('audio-tone 300 698' + '\r').encode('ascii'))
	time.sleep(0.35)
	#   beep(cH, 150);
	ser.write(('audio-tone 100 523' + '\r').encode('ascii'))
	time.sleep(0.15)
	#   beep(gS, 500);
	ser.write(('audio-tone 450 415' + '\r').encode('ascii'))
	time.sleep(0.50)
	#   beep(f, 350);
	ser.write(('audio-tone 300 349' + '\r').encode('ascii'))
	time.sleep(0.35)
	#   beep(cH, 150);
	ser.write(('audio-tone 100 523' + '\r').encode('ascii'))
	time.sleep(0.15)
	#   beep(a, 650);
	ser.write(('audio-tone 600 440' + '\r').encode('ascii'))
	time.sleep(0.65)

	time.sleep(0.35)