'''
https://www.careercup.com/question?id=5638939143045120

Given many coins of 3 different face values, print the combination sums of the coins up to 1000. Must be printed in order. 

eg: coins(10, 15, 55) 
print: 
10 
15 
20 
25 
30 
. 
. 
. 
1000

'''

def coinssum(c1, c2, c3):
	sums = {}
	sums[0] = True
	for s in range(1, 1000):
		if s-c1 in sums || s-c2 in sums || s-c3 in sums:
			sums[s] = True
			print s 	
