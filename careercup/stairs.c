#include <stdio.h>

void waysstairs(int steps) {
	if (steps == 0)
		return 1;
	if (steps < 0)
		return 0;
	return waysstairs(steps-1) + waysstairs(steps-2) + waysstairs(steps-3);
}

