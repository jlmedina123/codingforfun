'''
https://www.careercup.com/question?id=5156596605779968

Interleave list of lists in Java 
Example: 
input = [[1,2,3], [9, 0], [5], [-4,-5,-2,-3,-1]]; 
output = [1,9,5,-4,2,0,-5,3,-2,-3,-1]
'''

def listoflists(seq):
	sol = []
	longest = 0
	for l in seq:
		if len(l) > longest:
			longest = len(l)
	for i in range(longest):
		for l in seq:
			if len(l) > i:
				sol.append(l[i])
	return sol


