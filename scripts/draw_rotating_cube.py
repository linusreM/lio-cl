
import keyboard

import serial

import time

import numpy as np
class Point:
    def __init__(self,x, y, z):
        self.x = x
        self.y = y
        self.z = z

class Polygon:
	def __init__(self,p1, p2, p3):
		self.p1 = p1
		self.p2 = p2
		self.p3 = p3


def rotate_3d(point, degx, degy, degz):
	x = point.x
	y = point.y
	z = point.z

	#rotate x
	ry = (y*np.cos(np.deg2rad(degx))) + (z*np.sin(np.deg2rad(degx)))
	rz = (-y*np.sin(np.deg2rad(degx))) + (z*np.cos(np.deg2rad(degx)))
	y = ry
	z = rz

	#rotate y
	rx = (x*np.cos(np.deg2rad(degy))) - (z*np.sin(np.deg2rad(degy)))
	rz = (x*np.sin(np.deg2rad(degy))) + (z*np.cos(np.deg2rad(degy)))
	x = rx
	z = rz

	#rotate z
	rx = (x*np.cos(np.deg2rad(degz))) + (y*np.sin(np.deg2rad(degz)))
	ry = (-x*np.sin(np.deg2rad(degz))) + (y*np.cos(np.deg2rad(degz)))


	return Point(rx, ry, rz)

def normal(tri):
	

	p1 = Point(tri.p2.x - tri.p1.x, tri.p2.y - tri.p1.y, tri.p2.z - tri.p1.z)
	p2 = Point(tri.p3.x - tri.p1.x, tri.p3.y - tri.p1.y, tri.p3.z - tri.p1.z)

	return normalize(Point((p1.y*p2.z) - (p1.z*p2.y), (p1.z*p2.x) - (p1.x*p2.z) ,(p1.x*p2.y) - (p1.y*p2.x)))

def normalize(v):
	mag = length(v)
	return Point(v.x/mag, v.y/mag, v.z/mag)

def length(v):
	return np.sqrt((v.x*v.x)+(v.y*v.y)+(v.z*v.z))	

def dot(v1, v2):
	return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z)

light = Point(-1, 1, 0.5)

vert = [Point(-1,-1, 1),
		Point(-1, 1, 1),
		Point( 1, 1, 1),
		Point( 1,-1, 1),
		Point(-1,-1,-1),
		Point(-1, 1,-1),
		Point( 1, 1,-1),
		Point( 1,-1,-1)]

mesh = [Polygon(vert[0],vert[1],vert[3]),
		Polygon(vert[3],vert[1],vert[2]),
		Polygon(vert[1],vert[0],vert[4]),
		Polygon(vert[5],vert[1],vert[4]),
		Polygon(vert[3],vert[4],vert[0]),
		Polygon(vert[3],vert[7],vert[4]),
		Polygon(vert[3],vert[2],vert[7]),
		Polygon(vert[7],vert[2],vert[6]),
		Polygon(vert[2],vert[1],vert[5]),
		Polygon(vert[2],vert[5],vert[6]),
		Polygon(vert[7],vert[5],vert[4]),
		Polygon(vert[7],vert[6],vert[5])]

ser = serial.Serial("/dev/ttyACM0")

time.sleep(1)

ser.write(('k' + '\r').encode('ascii'))

time.sleep(0.2)

ser.write(('lcd-init' + '\r').encode('ascii'))
time.sleep(0.1)

scale = 100
color_scale = 0.12

offset_y = 40
offset_x = 80

offset_z = 5
intensity_offset = 8


while True:
	for tri in mesh:

		bf_normal = normal(Polygon(Point(tri.p1.x/(tri.p1.z + offset_z), tri.p1.y/(tri.p1.z + offset_z), (tri.p1.z + offset_z)), Point(tri.p2.x/(tri.p2.z + offset_z), tri.p2.y/(tri.p2.z + offset_z), (tri.p2.z + offset_z)), Point(tri.p3.x/(tri.p3.z + offset_z), tri.p3.y/(tri.p3.z + offset_z), (tri.p3.z + offset_z))))
		intensity = (dot(normal(tri), normalize(light)) + 1)/2
		backface = -dot(bf_normal, Point(0,0,1))

		if(backface < 0):
			color = ((64*32)*int(16.0 * intensity * (np.sin((time.time()*color_scale)+((2.0*np.pi)/3.0))+ 1.0))) + ((32)*int(32.0 * intensity *(np.sin((time.time()*color_scale)+((4.0*np.pi)/3.0)) + 1.0))) + int(16.0 * intensity *(np.sin(time.time()*color_scale) + 1.0))
			color %= 65535
			ser.write(('lcd-tri {:x} {} {} {} {} {} {}'.format(color, int(((tri.p1.x/(tri.p1.z+offset_z))*scale)+offset_x),\
																  int(((tri.p1.y/(tri.p1.z+offset_z))*scale)+offset_y),\
																  int(((tri.p2.x/(tri.p2.z+offset_z))*scale)+offset_x),\
																  int(((tri.p2.y/(tri.p2.z+offset_z))*scale)+offset_y),\
																  int(((tri.p3.x/(tri.p3.z+offset_z))*scale)+offset_x),\
																  int(((tri.p3.y/(tri.p3.z+offset_z))*scale)+offset_y)) + '\r').encode('ascii'))
		time.sleep(0.001)

	for tri in mesh:
		tri.p1 = rotate_3d(tri.p1, 5.3, 2.57, 7.5);
		tri.p2 = rotate_3d(tri.p2, 5.3, 2.57, 7.5);
		tri.p3 = rotate_3d(tri.p3, 5.3, 2.57, 7.5);
	print(" ")
	time.sleep(0.2)

	ser.write(('lcd-rect b 40 0 120 80' + '\r').encode('ascii'))


	
	