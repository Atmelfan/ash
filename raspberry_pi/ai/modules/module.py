import os, time
import threading, Queue
import logging
from logging.handlers import RotatingFileHandler

class Event(object):
	"""docstring for Event"""
	def __init__(self, name):
		super(Event, self).__init__()
		self.name = name

	# Returns true if this event is still valid (for example, face is still being tracked)
	# False if the event has become invalig (for example, face has been lost)
	def valid(self):
		return False

		

class Module(threading.Thread):
	"""docstring for Module"""

	drun = False

	def __init__(self, events, args):
		super(Module, self).__init__()
		self.stop = threading.Event()
		self.events = events
		self.args = args

		self.logger = logging.getLogger(self.__class__.__name__)
		self.logger.setLevel(logging.DEBUG)

		fh = RotatingFileHandler('logs/%s.log'%self.__class__.__name__, maxBytes=512*1024, backupCount=0)
		fh.setLevel(logging.DEBUG)
		
		formatter = logging.Formatter('%(asctime)s [%(name)s] %(levelname)s: %(message)s')
		fh.setFormatter(formatter)
		self.logger.addHandler(fh)

	def run(self):
		while not self.stop.is_set():
			try:
				if not self.drun:
					event = self.update()
					if event != None:
						self.events.put()
			except Queue.Empty:
				continue

	def join(self, timeout=None):
		self.stop.set()
		super(Module, self).join(timeout)

	# Update module
	def update(self):
		return None

	def pause(self, drun):
		if drun:
			self.logger.info('Paused')
		else:
			self.logger.info('Unpaused')

		self.drun=drun

		return self.drun

