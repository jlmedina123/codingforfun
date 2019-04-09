'''Three strings say A,B,C are given to you. Check weather 3rd string is interleaved from string A and B. 
Ex: A="abcd" B="xyz" C="axybczd". answer is yes. o(n)
https://www.careercup.com/question?id=14539805
'''

def interleaved(a, b, c):
	clist = list(c)
	for l in a:
		if l in clist:
			clist.remove(l)
	print "".join(clist)
	if b in "".join(clist):
		return True
	else:
		return False
A="abcd"
B="xyz"
C="axybczd"
print interleaved(A, B, C)

