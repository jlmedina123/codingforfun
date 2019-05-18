'''
https://www.techiedelight.com/longest-common-subsequence/

longest subsequence that is common in two sequences

The longest common subsequence (LCS) is the problem of finding the longest subsequence that is present in given two sequences in the same order. i.e. find a longest sequence which can be obtained from the first original sequence by deleting some items, and from the second original sequence by deleting other items

The problem differs from problem of finding common substrings. Unlike substrings, subsequences are not required to occupy consecutive positions within the original sequences
'''

def lcs(a, b, n, m):
	if m == 0 || n == 0:
		return 0
	if (a[n-1] == b[m-1]):
		return lcs(a, b, n-1, m-1) + 1;
	return max(lcs(a, b, n-1, m), lcs(a, b, n, m-1)




