
'''
Push all the zero's of a given array to the end of the array. In place only. Ex 1,2,0,4,0,0,8 becomes 1,2,4,8,0,0,0
https://www.careercup.com/question?id=12986664
'''


def pushzeros(seq):
	zi = 0
	for i in range(len(seq)):
		if seq[i] != 0:
			seq[zi] = seq[i]
			zi+=1
	for i in range(zi, len(seq)):
		seq[i] = 0
	print seq

seq = [5,0,1,2,0,4,0,0,8]
print seq
pushzeros(seq)
