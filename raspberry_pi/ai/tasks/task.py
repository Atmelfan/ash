class Task(object):
	"""docstring for Task"""
	def __init__(self, event, priority, name):
		super(Task, self).__init__()
		self.event = event
		self.priority = priority
		self.name = name
	
	def get_priority(self):
		return self.priority

	