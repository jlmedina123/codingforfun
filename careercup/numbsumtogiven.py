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

https://jlmedina123.wordpress.com/2014/05/27/numbers-that-sum-to-given-number/

'''

def sumnumbers(n, seq = []):
	results = []
	if (n == 0):
		return [seq]
	i = n-1
	while i > 0:
		seq += [i]
		results+=sumnumbers(n-i, seq)
		i-=1
	return results

print sumnumbers(5)
