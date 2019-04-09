'''
Given a string (for example: "a?bc?def?g"), write a program to generate all the possible strings by replacing ? with 0 and 1. 
Example: 
Input : a?b?c? 
Output: a0b0c0, a0b0c1, a0b1c0, a0b1c1, a1b0c0, a1b0c1, a1b1c0, a1b1c1.

https://www.careercup.com/question?id=5192571630387200

'''

def replaceqmark(strs):
	s = list(strs)
	if len(s) == 0:
		return ""
	results = []
	for i in range(len(s)):
		if s[i] == "?":
			tails = replaceqmark(s[i+1:])
			for tail in tails:
				results.append(s[0:i-1] + [0] + tail)
				results.append(s[0:i-1] + [1] + tail)
	return results

a = "a?b?c?"
print replaceqmark(a)
