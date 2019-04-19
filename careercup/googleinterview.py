'''
all paths from (0,0) to (n,m)
all subsets of a set
all permutations of a string
all combination of numbers  that sum to given number
all combinations of coins (1, 5, 10, 25) to have x cents
write a function that gives a list of all prime numbers smaller than the given number
write a function that, given a list of strings, returns a list of lists with strings that belong to the same ROT-n class. Two strings belong to the same ROT-n class if the characters in one string can be obtained by summing an offset (n) to the characters of another string. For example: ‘ace’ and ‘bdf’ are both ROT-1
write a function that, given two strings, returns true if one of the strings, or its anagram, is substring of the other
'''


'''
Calculate the number of ways a robot can get from a position 0,0 to a position X,Y (without getting out of the board with corners 0,0 and X,Y) if the robot can only move either 1 position to right, or 2 up.
'''
def numbpaths(n, m):
	if n == 0 && mm == 0:
		return 1
	if n < 0 || m < 0:
		return 0
	return numbpaths(n-1, m) + numbpaths(n, m-2)

def getpaths(n, m):
	if n == 0 && mm == 0:
		return []
	if n >= 0 && m >=0:
		return
	paths = []
	paths.append([n,m])
	# TODO

def powersets(seq):
	if len(seq) == 0:
		return []
	res = []
	head = seq[0]
	tail = seq[1:]
	tails = powersets(tail)
	for t in tails:
		res.append([head] + t)
		res.append(t)
	return res

def allperm(word):
	if len(word) <= 1:
		return [word]
	head = word[0]
	tail = word[1:]
	res = []
	perms = allperm(tail)
	for perm in perms:
		for i in range(len(word)):
			res.append(perm[:i] + head + perm[i:])
	return result

def allnumberssumtonum(n, seq = []):
	if n == 0:
		return [seq]
	if n > 0:
		results = []
		i = 1
		while n - i > 0:
			results.append(sumnumber(n-i, seq+[i])
			i+=1
		return results

'''
write a function that, given a list of strings, returns a list of lists with strings that belong to the same ROT-n class. Two strings belong to the same ROT-n class if the characters in one string can be obtained by summing an offset (n) to the characters of another string. For example: ‘ace’ and ‘bdf’ are both ROT-1
'''

def rotn(seqs):
	i = 0
	weight = {}
	for l in "abcdefghijklmnopqrstuvwxyz":
		weight[l] = i
		i+=1
	res = []
	for i in range(0, len(seqs)):
		for j in range(0, len(seqs)):
			n = weight[seq[i][0]] - weight[seq[j][0]]
			for k in range(len(seq[i])):
				if weight[seq[i][k]] - weight[seq[j][k]] != n
					break

	# something is wrong....

'''
write a function that, given two strings, returns true if one of the strings, or its anagram, is substring of the other
'''
def subanagram(stra, strb):
	if "".join(sorted(stra)) in "".join(sorted(strb)):
		return True
	else
		return False

'''
print list of prime numbers under number 
'''
def listprimes(k):
	if k < 1:
		return []
	if k < 4:
		return range(1, 4)
	for i in range(2, k):
		for j in range(2, i):
			if i % j == 0:
				break
		if j == i:
			result.append(i)
	return results
		
