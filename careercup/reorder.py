'''
We have an array of objects A and an array of indexes B. Reorder objects in array A with given indexes in array B. Do not change array A's length. 

example:


var A = [C, D, E, F, G];
var B = [3, 0, 4, 1, 2];

sort(A, B);
// A is now [D, F, G, C, E];

https://www.careercup.com/question?id=5756151524229120

'''

def reorder(a, b):
	for i in range(len(b)):
		# a[0] <-> a[b[0]], b[0] <-> b[b[0]]
		t = i
		a[i],a[b[i]] = a[b[i]],a[i] 
		b[i],b[b[i]] = b[b[i]], b[i]

A = ["C", "D", "E", "F", "G"]
B = [3, 0, 4, 1, 2]
print A
reorder(A,B)
print A
