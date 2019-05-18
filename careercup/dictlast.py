"""
Write a new data structure, "Dictionary with Last" 

Methods: 
set(key, value) - adds an element to the dictionary 
get(key) - returns the element 
delete(key) - removes the element 
last() - returns the last key that was added or read. 

In case a key was removed, last will return the previous key in order.
"""


class Dictwithlast:
	def __init__(self):
		self._dict = {}
		self._stack = []
	def set(self, key, value):
		self._dict[key] = value
		self._stack.append(key)
	def get(self, key):
		self._stack.append(key)
		return self._dict[key]
	def delete(self, key):
		del self._dict[key]
		if self._stack[-1] == key:
			self._stack.pop()
	def last(self):
		return self._stack[-1]
			

