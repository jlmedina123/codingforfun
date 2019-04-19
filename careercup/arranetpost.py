'''
Give you an array which has n integers,it has both positive and negative integers.Now you need sort this array in a special way.After that,the negative integers should in the front,and the positive integers should in the back.Also the relative position should not be changed. 
eg. -1 1 3 -2 2 ans: -1 -2 1 3 2. 
o(n)time complexity and o(1) space complexity is perfect.
'''

def sortspecial(seq):
	negs = []
	pos = []
	for e in seq:
		if e < 0:
			negs.append(e)
		else:
			pos.append(e)
	for i in range(len(negs))
		seq[i] = negs[i]
	for j in range(len(pos))
		seq[i+j] = pos[j]


