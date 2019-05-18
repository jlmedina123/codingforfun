'''Write a function, for a given number, print out all different ways to
 make this number, by using addition and any number equal to or smaller 
than this number and greater than zero.
 
For example, given a 5, we have the following different ways to
 make up 5:
 
1st: 1, 1, 1, 1, 1
2nd: 1, 4
3rd : 1, 1, 1, 2
4th : 1, 1, 3
5th : 2, 3
6th : 1, 2, 2
7th : 5
 
Print out one case at a time in its own line, assuming the line is 
sufficiently long.
'''

def sumnumber(num, seq = []):
	if num == 0:
		return seq
	res = []
	for i in range(1, num):
		seqb = seq + [i]
		res+= sumnumber(num-1,seqb)
