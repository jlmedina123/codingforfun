
def primes(n):
	results = [2, 3]
	for i in range(4, n):
		for j in range(2, i):
			if i % j == 0:
				break
		if j == i-1:
			results.append(i)
	return results

print primes(10)	
