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


""" set methods:

len(s)	 	number of elements in set s (cardinality)
x in s	 	test x for membership in s
x not in s	 	test x for non-membership in s
s.issubset(t)	s <= t	test whether every element in s is in t
s.issuperset(t)	s >= t	test whether every element in t is in s
s.union(t)	s | t	new set with elements from both s and t
s.intersection(t)	s & t	new set with elements common to s and t
s.difference(t)	s - t	new set with elements in s but not in t
s.symmetric_difference(t)	s ^ t	new set with elements in either s or t but not both
s.copy()	 	new set with a shallow copy of s

s.update(t)	s |= t	return set s with elements added from t
s.intersection_update(t)	s &= t	return set s keeping only elements also found in t
s.difference_update(t)	s -= t	return set s after removing elements found in t
s.symmetric_difference_update(t)	s ^= t	return set s with elements from s or t but not both
s.add(x)	 	add element x to set s
s.remove(x)	 	remove x from set s; raises KeyError if not present
s.discard(x)	 	removes x from set s if present
s.pop()	 	remove and return an arbitrary element from s; raises KeyError if empty
s.clear()	 	remove all elements from set s

"""
def coinssum(c1, c2, c3):
	sums = set()
	sums.add(0)
	for s in range(1, 1000):
		if (s-c1 in sums) or (s-c2 in sums) or (s-c3 in sums):
			sums.add(s)
			print s

coinssum(10, 15, 55)	
