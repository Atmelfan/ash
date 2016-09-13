import os
import sys
from gmd import Gmd
import threading, Queue
import logging
from logging.handlers import RotatingFileHandler
import curses
import shlex
import argparse

def alert(title, message):
	h=5
	w=32+len(message)
	size = screen.getmaxyx()
	win = curses.newwin(h, w, size[0]/2-h/2, size[1]/2-w/2)
	win.border(0)
	win.addstr(0, 1, title)
	s="Alert"
	win.addstr(0, w-1-len(s), s)
	win.addstr(h/2, w/2-len(message)/2, message)
	curses.beep()
	win.getch()

class UI_tab(object):
	"""docstring for UI_tab"""
	needs_attention=False
	def __init__(self, name):
		super(UI_tab, self).__init__()
		self.name = name

	def update(self):
		pass

	def command(self, cmd):
		pass

	def input(self, x):
		pass

class UI_terminal(UI_tab):
	"""docstring for UI_log"""
	lines=['(c) GPA Robotics 1989']

	def __init__(self):
		super(UI_terminal, self).__init__("Terminal")

	def update(self):
		ty=0
		for s in self.lines[-(screen.getmaxyx()[0]-2):]:
			screen.addstr(1+ty, 2, s)
			ty=ty+1

	def msg(self,s):
		self.lines.append(s)

	def command(self, cmd):
		if cmd.startswith('@'):
			cmd=cmd[1:]
		else:
			self.msg('ASH:~$ ' + cmd)
		args=shlex.split(cmd)
		if args[0]=="help":
			self.msg("----------CUI KEYS----------")
			self.msg("LEFT,RIGHT - Previus/next tab")
			self.msg("ALPHAMERIC,\'/\' - Opens command line")
			self.msg("----------COMMANDS----------")
			self.msg("help - This command doesn't really need an explanation don't you think?")
			self.msg("clear - Clear terminal")
			self.msg("echo <message...> - Echo message to terminal")
			self.msg("lsmod - List available modules")
			self.msg("pause <module> <true|false> - pause/run specified module")
			self.msg("//END OF HELP")
		elif args[0]=="clear":
			self.lines=[]
		elif args[0]=="echo":
			self.msg(" ".join(args[1:]))
		elif args[0]=="lsmod":
			self.msg("<module name>\t<Paused>")
			for m in modules:
				self.msg('%s\t%s'%(m, m.drun))
		elif args[0]=="pause":
			if len(args)>2:
				for mod in modules:
					if mod.__class__.__name__==args[1]:
						try:
							t = mod.pause(args[2] in ['true', '1', 't', 'y', 'yes', 'yeah', 'yup', 'certainly', 'uh-huh', 'please', 'maybe?'])
							if t:
								self.msg("Paused")
							else:
								self.msg("Unpaused")
						except Exception, e:
							self.msg(args[2]+' is not a boolean...')
			else:
				self.msg('Usage: pause <module> <true|false>')
		elif args[0]=="alert":
			if len(args)>2:
				alert(args[1], args[2])
			else:
				self.msg('Usage: alert <title> <message>')
		else:
			self.msg('Unknown command(try \'help\'?): ' + args[0])

		#self.lines.append(cmd)

tasks = []

class TaskManager(threading.Thread):
	"""docstring for TaskManager"""
	def __init__(self, queue):
		super(TaskManager, self).__init__()
		self.queue = queue
		self.stop = threading.Event()
	
	def run(self):
		while not self.stop.is_set():
			try:
				self.queue.get_nowait()
			except Exception, e:
				pass
	
	def join(self, timeout=None):
		self.stop.set()
		super(TaskManager, self).join(timeout)

modules = []

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Core')
	parser.add_argument('--gui', action='store_true', help='Run curses interface (WARNING! BUGGED AS HELL AND LEAKS MEMORY AS IF WRITTEN BY von Braun)')
	parser.add_argument('config', help='Core configuration file (gmd format)')
	args = parser.parse_args()

	logger = logging.getLogger('CORE')
	logger.setLevel(logging.DEBUG)

	fh = RotatingFileHandler('logs/_core.log', maxBytes=512*1024, backupCount=5)
	fh.setLevel(logging.DEBUG)

	ch = logging.StreamHandler()
	ch.setLevel(logging.INFO)
	
	formatter = logging.Formatter('%(asctime)s [%(name)s] %(levelname)s: %(message)s')
	ch.setFormatter(formatter)
	fh.setFormatter(formatter)
	logger.addHandler(fh)
	#logger.addHandler(ch)

	g = Gmd()
	queue = Queue.Queue()
	taskmanager = TaskManager(queue)
	taskmanager.start()
	with open(args.config, 'r+') as _input:
		logger.info("Reading config file %s..." % sys.argv[1])
		g.parse(_input)
		for m in g.findchild("modules").children:
			try:
				module = __import__("modules.%s" % m.args[0], globals(), locals(), fromlist=["*"])
				#for item_name in dir(module):
				try:
					tmp = getattr(module, m.args[0])(queue, m)
					logger.info("Running %s %s !" % (m.args[0],str(m.args[1:])))
					#print(tmp)
					tmp.start()
					modules.append(tmp)
				except:
					logger.exception('Failed to start!')
			except ImportError:
				logger.warn("Module %s does not exist!" % m.args[0])
	
	try:
		if args.gui:
			tabs=[UI_terminal(),UI_tab("Modules"),UI_tab("Tasks"),UI_tab("Database")]
			title=g.findchild("title").args[1]
			
			# #curses.savetty()
			x=0
			index=0
			while True:
			 	screen = curses.initscr()
			 	screen.keypad(1)
			 	screen.erase()
			 	screen.border(0)
			 	screen.addstr(0, screen.getmaxyx()[1]-1-len(title), title)
			 	t=ti=0
			 	for tab in tabs:
			 		screen.addstr(0, t, '|')
			 		if ti==index:
			 			screen.addstr(0, t+1, '%s'%tab.name)
			 			tab.needs_attention=False
			 		else:
			 			screen.addstr(0, t+1, '%s'%tab.name, curses.A_UNDERLINE | curses.A_STANDOUT if tab.needs_attention else curses.A_UNDERLINE)

			 		t = t+1+len(tab.name)
			 		ti = ti+1
			 	screen.addstr(0, t, '|')

			 	tabs[index].update()
			 	#screen.refresh()

			 	#curses.noecho()
			 	#x = screen.getch()
			 	#curses.echo()

			 	if x==curses.KEY_LEFT:
			 		if index>0:
			 			index=index-1
			 	elif x==curses.KEY_RIGHT:
			 		if index<len(tabs)-1:
			 			index=index+1
			 	elif x < 128 and (unichr(x).isalpha() or unichr(x)=='/'):
			 		curses.ungetch(x)
			 		screen.addstr(screen.getmaxyx()[0]-1, 1, 'ASH:~$ ')
			 		cmd=screen.getstr(screen.getmaxyx()[0]-1, 8)
			 		if cmd:
			 			tabs[index].command(cmd)
			 	elif x != ord('*'):
			 		tabs[index].input(x)
		else:
			print('No gui')
			while True:
				pass
	except KeyboardInterrupt:
	    pass
	except Exception:
		logger.exception('Unexpected exception');
	finally:
		if args.gui:
			pass
			#curses.resetty()
			#curses.endwin()
	for module in modules:
		module.join()
	taskmanager.join()
	logger.debug(modules)
