'''
https://www.techiedelight.com/longest-common-substring-problem/


'''

def lcsubstring(a, b):
	table = []
	for i in range(1, len(a)):
		for j in range(1, len(b)):
			table[i][j] = 0
			if 
