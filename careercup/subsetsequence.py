"""Given an int array which might contain duplicates,
find the largest subset of it which form a sequence.
Eg. {1,6,10,4,7,9,5}
then ans is 4,5,6,7
Sorting is an obvious solution. Can this be done in O(n) time"""

''' https://www.careercup.com/question?id=11070934'''

def subsetsequence(seq):
	table = {} ''' table[element] = {beg, end} '''
	for n in seq:
		if n in table:
			if table[n][0] == n-1:
				table[n][0] = n
			if table[n][1] == n+1:
				table[n][1] = n
			if table[n][1] - table[n][0] > maxseq:
				maxseq = table[n][1] - table[n][0]
				maxbeg = table[n][0]
		else:
			table[n] = [n, n]
	
