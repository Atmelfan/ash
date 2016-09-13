#!/home/ash/.virtualenvs/cv3/bin/python
from picamera.array import PiRGBArray
from picamera import PiCamera
import math
import time
import numpy as np
import cv2
import sound2
import serial

CAMERA_WIDTH=320
CAMERA_HEIGHT=240

lasttarget = -20
lastmessage = -20

ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=1)
print('Using %s' % ser.name)
face_cascade = cv2.CascadeClassifier('opencv/data/haarcascades/haarcascade_frontalface_default.xml')
camera = PiCamera()
camera.resolution = (CAMERA_WIDTH, CAMERA_HEIGHT)
camera.framerate = 32
camera.vflip = True
camera.hflip = True
rawCapture = PiRGBArray(camera, size=(CAMERA_WIDTH, CAMERA_HEIGHT))
sound2.play('autosearch')
camera.start_preview()
target_count = 0
ser.write(b'light 0 0 0\n\r')
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
	image = frame.array
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	faces = face_cascade.detectMultiScale(gray, 1.3, 5)
	i = len(faces)
	if(i == 0):
		camera.led = False
	else:
		camera.led = True
		ser.write(b'light 1 1 1\n\r')
	if(i > target_count):
		target_count = i
		if(time.clock() - lastmessage > 10):
			sound2.play('active')
			lastmessage = time.clock()
			print('echo! %f '%(time.clock()))
	if(i < target_count):
		target_count = i
		if(time.clock() - lastmessage > 20):
			sound2.play('search')
			lastmessage = time.clock()
	for (x,y,w,h) in faces:
		lasttarget = time.clock()
		#print (x,y)
		yaw = (53.0/180.0)*(math.pi/CAMERA_WIDTH)*((x + w/2)-CAMERA_WIDTH/2)*2
		pitch = -(41.0/180.0)*(math.pi/CAMERA_HEIGHT)*((y+ h/2)-CAMERA_HEIGHT/2)*2+math.pi/4
		ser.write(b'lookat %f %f\n\r' % (yaw, pitch))
		print('lookat %f %f\n\r' % (yaw, pitch))
		print(lastmessage)
		print(time.clock())
	rawCapture.truncate(0)
	if(time.clock() - lasttarget > 5):
		ser.write(b'lookat 0 0\n\r')
		time.sleep(0.010)
		ser.write(b'light 0 0 0\n\r')
 

