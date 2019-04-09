
'''
Given two strings a and b, find whether any anagram of string a is a sub-string of string b. For eg: 
if a = xyz and b = afdgzyxksldfm then the program should return true.
https://www.careercup.com/question?id=5389078581215232
'''

def anagramsub(a, b):
	asorted = "".join(sorted(a))
	for i in range(len(b) - len(a)):
		if asorted == "".join(sorted(b[i:i+len(a)])):
			print "True"
			return
	return "False"
		
a = "xyz"
b = "afdgzyxksldfm"

anagramsub(a, b)
