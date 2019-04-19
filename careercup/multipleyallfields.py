'''
input [2,3,1,4] 
output [12,8,24,6] 

Multiply all fields except it's own position. 

Restrictions: 
1. no use of division 
2. complexity in O(n)
'''

def multall(seq):
	front = []
	back = []
	front[0] = 1
	back[len(seq)-1] = 1
	for i in range(1, len(seq)-1):
		front[i] = front[i-1] * seq[i]
	for i in range(len(seq)-2, 0):
		back[i
