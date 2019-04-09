
def subsets(seq):
	if len(seq) == 0:
		return []
	results = []
	head = seq[0]
	tails = subsets(seq[1:])
	for tail in tails:
		results.append([head] + tail)
		results.apend(tail)
	return results
	
