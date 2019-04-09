'''
Design an algorithm that, given a list of n elements in an array, finds all the elements that appear more than n/3 times in the list. The algorithm should run in linear time ( n >=0 ) 

You are expected to use comparisons and achieve linear time. No hashing/excessive space/ and don't use standard linear time deterministic selection algo

https://www.careercup.com/question?id=14099679

'''

def elementsntimes(seq):
	m = {}
	ntimes = []
	for s in seq:
		if s not in m:
			m[s] = 1
		else:
			m[s] += 1
	for key in m.keys():
		if m[key] > int(len(seq)/3):
			ntimes.append(key)
	print ntimes

''' in c I could use a binary search tree '''	

a = [1, 3, 6, 4, 5, 5, 5, 7, 5, 2]
print a
elementsntimes(a)
