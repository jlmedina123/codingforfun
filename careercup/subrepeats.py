"""
Given a string (1-d array) , find if there is any sub-sequence that repeats itself. 
Here, sub-sequence can be a non-contiguous pattern, with the same relative order. 

Eg: 

1. abab <------yes, ab is repeated 
2. abba <---- No, a and b follow different order 
3. acbdaghfb <-------- yes there is a followed by b at two places 
4. abcdacb <----- yes a followed by b twice 

The above should be applicable to ANY TWO (or every two) characters in the string and optimum over time. 

In the sense, it should be checked for every pair of characters in the string.

https://www.careercup.com/question?id=5931067269709824
"""
'''
def subs(string):
	seq = list(string)
	for i in range(0, len(seq)-1):
		left = seq
		left.pop(i)
		left.pop(i+1)
		if seq[i, i+1] in "".join(order(left)):
			return True
'''

def subs2(string):
	elements = {}
	for i, val in enumerate(string):
		if val not in elements.keys():
			elements[val] = [i]
		else:
			elements[val].append(i)
	for i, val in enumerate(string):
		if i == len(string)-1:
			break
		valnext = string[i+1]
		if (len(elements[val]) > 1) and (len(elements[valnext] > 1)):
			if (elements[valnext][1] > elements[val][1]):	
				print "%c %c repeated" % (val, valnext)
	print done


def subs3(seq):
        elem = {}
        for i, val in enumerate(seq):
                if val not in elem.keys():
                        elem[val] = [i]
                else:
                        elem[val].append(i)
        for i, val in enumerate(seq):
                if len(elem[i]) < 2:
                        continue
                for j in range(i+1 ,len(seq)):
                        if (elem[i][0] > elem[j][0] and elem[i][1] > elem[j][1])
                                return [elem[i] + elem[j]]


def subs4(seq):
	rep = []
	repeated = ""
	for i in range(seq):
		rep[i] = 0
	for i in range(seq):
		rep[i]+=1
	for i in range(seq): 
		if rep[i] > 1:
			repeated.append(seq[i])
	# if palindrome ??
	if repeated[0:len(repeated)/2] == repeated[len(repeated)/2:len(repeated)]	
		return False
	else
		return True
print subs2("acbdaghfb")

