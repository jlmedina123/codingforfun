
'''
Given two strings a and b, find whether any anagram of string a is a sub-string of string b. For eg: 
if a = xyz and b = afdgzyxksldfm then the program should return true.
https://www.careercup.com/question?id=5389078581215232
'''

def anagramsubstring(a, b):
        asorted = "".join(sorted(a))
        for i in range(0, len(b)-len(a)):
                c = b[i:i+len(a)]
                csorted = "".join(sorted(c))
                if csorted == asorted:
                        return True
        return False

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
