from module import Module, Event
from picamera.array import PiRGBArray
from picamera import PiCamera
import math
import time
import numpy as np
import cv2
import sound2
import serial


class event_new_face(Event):
	"""docstring for Face_new"""
	last_face_time = 0

	def __init__(self, arg):
		super(Face_new, self).__init__(self.__class__.__name__)
		self.arg = arg

	def angle(self):
		return (0, 0)

	def is_visible(self):
		return False

	def valid(self):
		return last_face_time < 30

	def identity():
		return None

	def mood():
		return None

def detect_faces(image):
	pass

def identify_face(face_image):
	pass

def mood_face(face_image):
	pass


class camera(Module):
	"""docstring for mod_camera"""

	faces = []

	def __init__(self, queue, args):
		super(camera, self).__init__(queue, args)
		self.face_cascade = cv2.CascadeClassifier('opencv/data/haarcascades/haarcascade_frontalface_default.xml')
		self.camera = PiCamera()
		self.camera.resolution = (CAMERA_WIDTH, CAMERA_HEIGHT)
		self.camera.framerate = 32
		self.camera.vflip = True
		self.camera.hflip = True
		self.stream = picamera.array.PiRGBArray(camera)

	def update(self):
		camera.capture(stream, format='bgr')
		image = stream.array


		
		




