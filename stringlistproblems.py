"""
https://jlmedina123.wordpress.com/2014/05/27/stringlist-manipulation-in-python/

List and string operations in Python
    reverse:
        list1.reverse()
        list2 = reversed(list1)
        string2 = string1[::-1]
        string2 = "".join(reversed(string1))
    sort:
        list1.sort()
        list2 = sorted(list1)
        list = sorted(string)
        string2 = "".join(sorted(string1)) # capital first
 
    change case:
        strlower = string1.lower()
        strupper = string1.upper()
 
    is substring:
        if str1 in str2:
 
    reverse of substring:
        substr = str1[a:b]
        reversubstr = str1[b-1:a-1:-1]
"""
 
'''Given two strings a and b, find whether any anagram of string a
   is a sub-string of string b. For eg:
   if a = xyz and b = afdgzyxksldfm then the program should return true.
'''

def anagramsub(a, b):
        if "".join(sort(a)) in "".join(sort(b)):
                return True
        return False

'''Given a string, find whether it has any permutation of another string.
 For example, given "abcdefg" and "ba", it shuold return true,
 because "abcdefg" has substring "ab", which is a permutation of "ba"
'''
def substrper(a, b):
        if len(a) > len(b):     
                return False
        asort = sort(a)
        for i in range(len(a), len(b)):
                subb = b[i-len(a):i]
                if asort == (sort(subb)):
                        return True
        return False

''' Given two sequences of length N, find the max window of matching
patterns. For example, seq1 = "ABCDEFG", seq2 = "DBCAPFG", then
the max window is 4. (ABCD from seq1 and DBCA from seq2)
'''
def maxwindowmatch(a, b):
	tmax = 0
	imax = 0
	jmax = 0
	for i in range(a):
		t = 0
		for j in range(b):
			if sort(a[i:i+t]) == sort(b[j:j+t])
				t+=1
				if t > tmax:
					tmax = t
					imax = i
					jmax = j
			else:
				break
	return tmax

'''Imagine we have a large string like this
"ABCBAHELLOHOWRACECARAREYOUIAMAIDOINGGOOD" which contains
multiple palindromes within it, like ABCBA, RACECAR, ARA, IAMAI etc...
 Now write a method which will accept this large string and return
the largest palindrome from this string. If there are two palindromes
 which are of same size, it would be sufficient to just return any one
of them.
'''
def largestpalindrome(seq):
	maxsize = 0
	imax = 0
	largest = ""
	for i in range(1, len(seq)):
		t = 1
		while seq[i-1:-1:i-t] == seq[i+1:i+t]:
			if t > maxsize:
				maxsize = t
				imax = i
				largest = seq[i-1:i+t]
			t+=1
	return largest


