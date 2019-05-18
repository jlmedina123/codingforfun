'''
facebook-interview-questions6
of 6 votes
21 
Answers
FB On-site March 
Q: Find number of Islands. 
XXXOO 
OOOXX 
XXOOX 
Return 3 islands. 
1 1 1OO 
OOO2 2 
3 3OO 2 
Followup: If the board is too big to fit in memory, how to get the number?


 
Given a 2d array of 0s and 1s, 0 means water, 
1 means land, connected 1s form an island, 
count the number of islands on this map. 
01010 
01001 
01101 
returns 3 
'''

def explore(seq, i, j):
	if i < 0 || j < 0 || i > columns || j > rows:
		return
	if seq[i][j] == 0:
		return
	seq[i][j] == 2
	explore(seq, i+1, j)
	explore(seq, i-1, j)
	explore(seq, i, j+1)
	explore(seq, i, j-1)
	

def countislands(seq):
	islands=2
	rows = len(seq)
	columns = len(seq[0])
	for i in range(rows):
		for j in range(columns):
			if seq[i][j] == 1:
				explore(seq, i, j)
				islands+=1
	return islands

