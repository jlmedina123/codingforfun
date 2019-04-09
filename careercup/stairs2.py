
def stairs(n):
	if n == 0:
		return 1
	if n < 0:
		return 0
	return stairs(n-3) + stairs(n-2) + stairs(n-1)	
