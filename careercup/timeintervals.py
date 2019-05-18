"""
https://www.careercup.com/question?id=5687609083297792
Given a list of arrays of time intervals, write a function that calculates the total amount of time covered by the intervals. 
For example: 
input = [(1,4), (2,3)] 
return 3 
input = [(4,6), (1,2)] 
return 3 
input = {{1,4}, {6,8}, {2,4}, {7,9}, {10, 15}} 
return 11
"""

def timeintervals(seq):
	total = {}
	for inter in seq:
		start = inter[0]
		end = inter[1]
		for t in range(start, end):
			if t not in total:
				total[t] = True
	return len(total.keys())

print timeintervals([(1,4), (2,3)])
print timeintervals([(4,6), (1,2)])
