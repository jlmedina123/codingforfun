#include <stdio.h>

char * interleaved(char a[], char b[], char c[]) {
	int ia = 0, ib = 0, ic;
	for (ic = 0; c[ic]; ic++) {
		if (c[ic] == a[ia])
			ia++;
		else if (c[ic] == b[ib])
			ib++;
		else
			return "no";
	}
	return "yes";
}

int main(int argc, const char *argv[]) {
	printf("%s\n", interleaved("abcd", "xyz", "axybczd"));
	return 0;
}

