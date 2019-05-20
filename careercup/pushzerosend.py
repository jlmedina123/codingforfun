'''
https://www.careercup.com/question?id=5143433806086144

Move[inplace] the non zero elements at the one end(end of array) and return the numbers of non zero elements in output array 

Solution : https://www.geeksforgeeks.org/move-zeroes-end-array/
'''

def pushzeros(seq):
	zeroindex = len(seq)-1
	zeros=0
	for i in range(len(seq)):
		if seq[i] == 0:
			seq[i], seq[zeroindex] = seq[zeroindex], seq[i]
			zeros+=1
	print seq
	return len(seq)-zeros

pushzeros([1, 6, 3, 0, 6, 0, 5])
