import shlex
import sys
import re

class Gmd_tag(object):
	"""Gmd tag"""
	
	def __init__(self, args):
		super(Gmd_tag, self).__init__()
		self.args = args
		self.children=[]
		#print(args)

	def get(self, index):
		return self.args[index]

	def getchildren(self):
		return self.children

	def findchild(self, name):
		for c in self.children:
			if name==c.args[0]:
				return c
		return None

	def __getitem__(self, key):
		if isinstance(key, (int, long)):
			return self.args[key]
		elif isinstance(key, (int, long)):
			return findchild(key)
		else:
			raise IndexError

	

	def readchildren(self, _input):
		temp = _input.readline()
		while temp:
			temp = temp.split('#')[0].strip()
			if temp == 'end':
				return
			elif temp.startswith('#') or not temp:
				pass
			elif temp.endswith(':'):
				#print('!')
				t = Gmd_tag(shlex.split(temp[:-1]))
				t.readchildren(_input)
				self.children.append(t)
				#print('!2')
			else:
				t = Gmd_tag(shlex.split(temp))
				self.children.append(t)
			temp = _input.readline()
	
	def _print(self, tab=0):
		print('  '*tab + str(self.args))
		#print(self)
		for c in self.children:
			#print(c)
			c._print(tab + 1)

	regexp = re.compile(r'\s')
	def writechildren(self, _output, tab=0):
		_output.write('\t'*tab)
		for arg in self.args:
			if self.regexp.search(arg) is not None:
				_output.write('"' + arg + '" ')
			else:
				_output.write(arg + ' ')

		if len(self.children) == 0:
			_output.write('\n')
		else:
			_output.write(':\n')
			for c in self.children:
				c.writechildren(_output, tab + 1)
			_output.write('\t'*tab + 'end\n')

		


class Gmd(Gmd_tag):
	"""Gmd parser v0.1"""
	def __init__(self):
		super(Gmd, self).__init__([''])

	def parse(self, _input):
		self.args = [_input.name]
		line = _input.readline()
		while line:
			line = line.split('#')[0].strip()
			if line.startswith('#') or not line:
				pass
			elif line == 'end':
				print('Found an extra "end", lost anything?')
			elif line.endswith(':'):
				#print('!')
				t = Gmd_tag(shlex.split(line[:-1]))
				t.readchildren(_input)
				self.children.append(t)
				#print('!2')
			else:
				t = Gmd_tag(shlex.split(line))
				self.children.append(t)
			line = _input.readline()

	def dump(self, _output):
		for c in self.children:
			c.writechildren(_output, 0)

if __name__ == "__main__":
    g = Gmd()
    with open(sys.argv[1], 'r+') as _input:
    	g.parse(_input)
    	g._print()
    	g.dump(_input)

    	#print(len(g.children))