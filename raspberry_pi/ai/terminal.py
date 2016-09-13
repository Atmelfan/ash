#!/usr/bin/env python

from os import system
import curses
import shlex
import curses.textpad as textpad

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
	lines=['(c) GPA Robotics 1989',
	'Use L/R arrows to change tab, \'*\' to exit and any letter or \'/\' to enter a command!']

	def __init__(self):
		super(UI_terminal, self).__init__("Terminal")

	def update(self):
		ty=0
		for s in self.lines[-(screen.getmaxyx()[0]-2):]:
			screen.addstr(1+ty, 2, s)
			ty=ty+1

	def command(self, cmd):
		self.lines.append(cmd)
		
		

class UI_log(UI_tab):
	"""docstring for UI_log"""
	def __init__(self):
		super(UI_log, self).__init__("Log")
		self.needs_attention=True

	def update(self):
		screen.addstr(1, 2, "2016-04-05 22:00:56 WARN | Hello world!")

	def command(self, cmd):
		pass

class rule(object):
	"""docstring for rule"""
	conditions = ''
	commands = ''

	def __init__(self, name):
		super(rule, self).__init__()
		self.name = name
	
	def edit():
		pass

class UI_rules(UI_tab):
	"""docstring for UI_log"""

	
	rindex=0
	rules=[]

	def __init__(self):
		super(UI_rules, self).__init__("Event")

	def update(self):
		screen.addstr(1, 2, "Press \'+\' to create a new event, U/D to select, \'-\' to edit, DEL to delete")
		ty=0
		for s in self.rules[-(screen.getmaxyx()[0]-2):]:
			if ty==self.rindex:
				screen.addnstr(2+ty, 2, s.name, 20, curses.A_REVERSE)
				screen.addnstr(2+ty, 22, s.conditions, 20, curses.A_REVERSE)
			else:
				screen.addnstr(2+ty, 2, s.name, 20)
				screen.addnstr(2+ty, 22, s.conditions, 20)
			ty=ty+1

	def input(self, x):
		if x==ord('+'):
			size = screen.getmaxyx()
			win = curses.newwin(12, 62, size[0]/2-6, size[1]/2-31)
			win.border(0)
			win.addstr(0,1, 'Create new event: ')
			name = win.getstr(0,19,15)
			if name:
				r = rule(name)
				win.addstr(0, 19 + len(name), ' @ ')
				r.conditions = win.getstr(0, 19 + len(name) + 3, 25)
				editwin = curses.newwin(10, 60, size[0]/2-5, size[1]/2-30)
				editwin.clear()
				box = textpad.Textbox(editwin)
				curses.noecho()
				r.commands = box.edit()
				curses.echo()
				self.rules.append(r)
		elif x==ord('-'):
			size = screen.getmaxyx()
			win = curses.newwin(12, 62, size[0]/2-6, size[1]/2-31)
			r=self.rules[self.rindex]
			win.border(0)
			win.addstr(0,1, 'Edit event: ' + r.name + ' @ ')
			t = win.getstr(0, 15 + len(r.name), 25)
			while not t:
				t = win.getstr(0, 15 + len(r.name), 25)
			r.conditions = t
			editwin = curses.newwin(10, 60, size[0]/2-5, size[1]/2-30)
			editwin.clear()
			editwin.addstr(r.commands)
			box = textpad.Textbox(editwin)
			curses.noecho()
			r.commands = box.edit()
			curses.echo()
		elif x==curses.KEY_DOWN:
			if self.rindex < len(self.rules)-1:
				self.rindex=self.rindex+1
		elif x==curses.KEY_UP:
			if self.rindex > 0:
				self.rindex=self.rindex-1



title="ASH interface"
x = 0
index=0
tabs=[UI_terminal(),UI_log(),UI_rules(),UI_tab("Tasks"),UI_tab("Database"),UI_tab("Memory")]

while x != ord('*'):
	screen = curses.initscr()
	screen.keypad(1)
	# don't echo key strokes on the screen
	
	# read keystrokes instantly, without waiting for enter to ne pressed
	
	screen.clear()
	t=0
	ti=0
	screen.border(0)
	screen.addstr(0, screen.getmaxyx()[1]-1-len(title), title)
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

	screen.refresh()

	x = screen.getch()
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
	

curses.endwin()
