
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
 
def anagramsubstring(a, b):
    j = 0
    for i in reversed(range(len(b))):
        if b[i] == a[j]:
            j+=1
            if j == len(a):
                return True
        else:
            j = 0
    return False
 
def anagramsubstring2(a,b):
    arev = "".join(reversed(a))
    if arev in b: return True
    else: return False
 
 
'''Given a string, find whether it has any permutation of another string.
 For example, given "abcdefg" and "ba", it shuold return true,
 because "abcdefg" has substring "ab", which is a permutation of "ba"
'''
def substringpermutation(a, b):
    i = 0
    j = len(b)-1
    for c in a:
        if c == b[i]:
            i+=1
            if i == len(b):
                return True
        else: i = 0
 
        if c == b[j]:
            j-=1
            if j < 0:
                return True
 
        else: j = len(b)-1
    return False
 
def substringpermutation2(a, b):
    asort = "".join(sorted(a))
    for i in range(0, len(b) - len(a):
	if asort == "".join(sort(b[i:i+len(a)])):
	    return True
    return False

 
''' Given two sequences of length N, find the max window of matching
patterns. For example, seq1 = "ABCDEFG", seq2 = "DBCAPFG", then
the max window is 4. (ABCD from seq1 and DBCA from seq2)
https://www.careercup.com/question?id=18868666
'''
def maxmatching(seq1, seq2):
    window = ""
    maxwindowsize = 0
    for i in range(len(seq1)):
        # keep increasing window
        for j in range(i, len(seq2)):
            windowsize = j - i
             # move window through seq2 to see if there is match
            for k in range(len(seq2)-windowsize):
                if sorted(seq1[i:j]) == sorted(seq2[k:k+windowsize]):
                    if windowsize > maxwindowsize:
                        maxwindowsize = windowsize
                        window = seq1[i:j]
    return window

 
'''Imagine we have a large string like this
"ABCBAHELLOHOWRACECARAREYOUIAMAIDOINGGOOD" which contains
multiple palindromes within it, like ABCBA, RACECAR, ARA, IAMAI etc...
 Now write a method which will accept this large string and return
the largest palindrome from this string. If there are two palindromes
 which are of same size, it would be sufficient to just return any one
of them.
'''
def largestpalindrome(seq):
    largestsize = 0
    largestpalindrome = ""
    for i in range(len(seq)):
        for j in range(i, len(seq)):
            if seq[i:j] == seq[j-1:i-1:-1] and j-i > largestsize:
                largestpalindrome = seq[i:j]
                largestsize = j-i
 
    return largestpalindrome
 
a = "xyz"
b = "afdgzyxksldfm"
print anagramsubstring(a, b)
 
a = "abcdefg"
b = "ba"
print substringpermutation(a, b)
 
seq1 = "ABCDEFG"
seq2 = "DBCAPFG"
print maxmatching(seq1, seq2)
 
seq = "ABCBAHELLOHOWRACECARAREYOUIAMAIDOINGGOOD"
print largestpalindrome(seq)
