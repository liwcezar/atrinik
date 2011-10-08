## @file
## Implements the /console command.
##
## Only usable by DMs or those with the "console" command permission.

from Interface import Interface
import threading, code

## Version of the command.
__VERSION__ = "1.0"
## The beginning of the thread name.
__THREADNAME__ = "PyConsoleThread-"

## Handles the console data.
class PyConsole(code.InteractiveConsole):
	def write(self, data):
		self.inf_data.append(data)

## The actual function that is called in the per-player console thread.
def py_console_thread():
	import time, collections, sys

	# Sends an interface to the activator.
	def send_inf(activator, msg):
		inf = Interface(activator, None)
		inf.set_icon(activator.face[0])
		inf.set_title(activator.name + "'s Python Console")
		inf.set_text_input(prepend = "/console \"", allow_tab = True, allow_empty = True, cleanup_text = False, scroll_bottom = True)
		inf.add_msg("<font=mono 12>" + msg + "</font>")
		inf.finish()

	# Used to redirect stdout so that it writes the print() and the like
	# data to the interface instead of stdout.
	class stdout_inf:
		def __init__(self, inf_data):
			self._inf_data = inf_data

		def write(self, s):
			if s != "\n":
				self._inf_data += s.split("\n")

	# Create a ring buffer.
	inf_data = collections.deque(maxlen = 20)
	# Get the current thread.
	thread = threading.current_thread()

	# Create the console.
	console = PyConsole()
	console.inf_data = inf_data

	old_stdout = sys.stdout
	# Redirect stdout.
	sys.stdout = stdout_inf(inf_data)

	# Send the greeting message.
	inf_data.append("Atrinik Python Console v{}".format(__VERSION__))
	inf_data.append("Use exit() to exit the session.")
	send_inf(thread.activator, "\n".join(inf_data))

	# Now loop until we get killed.
	while not thread.killed():
		# Activator object is no longer valid, break out.
		if not thread.activator:
			break

		# Acquire the commands lost.
		thread.commands_lock.acquire()

		for command in thread.commands:
			if command == None:
				continue

			inf_data.append(">>> {}".format(command))
			console.push(command)

		# If there were any commands, send an interface and clear the
		# commands list.
		if thread.commands:
			send_inf(thread.activator, "\n".join(inf_data))
			thread.commands = []

		# Release the lock.
		thread.commands_lock.release()
		# Save CPU.
		time.sleep(0.30)

	sys.stdout = old_stdout

## Setups the console thread.
class PyConsoleThread(threading.Thread):
	def __init__(self, activator):
		# The thread name will have the activator's name for uniqueness.
		super(PyConsoleThread, self).__init__(name = __THREADNAME__ + activator.name, target = py_console_thread)
		self.activator = activator
		self.commands = []
		self.commands_lock = threading.Lock()
		self._kill = threading.Event()

	def kill(self):
		self._kill.set()

	def killed(self):
		return self._kill.is_set()

## Tries to find an existing console thread.
def find_thread():
	for thread in threading.enumerate():
		if thread.name == __THREADNAME__ + activator.name:
			return thread

def main():
	msg = WhatIsMessage()

	# The hidden prepend keyword of the text input also includes
	# double-quotes so that left whitespace is not stripped by the
	# server.
	if msg and msg.startswith("\""):
		msg = msg[1:]

	# Try to find the thread.
	thread = find_thread()

	if msg == "exit()":
		# Kill the existing thread.
		if thread:
			thread.kill()
			inf = Interface(activator, None)
			inf.dialog_close()

		return
	# Add the command to the existing thread's commands, if it exists.
	elif thread:
		thread.commands_lock.acquire()
		thread.commands.append(msg)
		thread.commands_lock.release()
		return

	# Thread doesn't exist yet, create it.
	thread = PyConsoleThread(activator)
	thread.start()

if activator.f_wiz or "console" in activator.Controller().cmd_permissions:
	main()