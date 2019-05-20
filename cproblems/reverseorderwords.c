#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void revword(char *l, char *r) {
	assert(l && r);
	while (*l && *r && l < r) {
		*l ^= *r;
		*r ^= *l;
		*l ^= *r;
		l++; r--;
	}
}

void rev(char *str) {
	assert(str);
	char *l = str;
	char *r = str;
	while (*r == ' ') { r++; }
        l=r;
	while (*r) {
		while (*r && *r != ' ') { r++; }
		revword(l, r-1);
		r++;
		l=r;
		while (*r == ' ') { r++; }
		l=r;
	}
	printf("-%s-\n", str);	// reverse words in string
	revword(str, r-1);
	printf("-%s-\n", str);	// reverse order of words (not working?)

}

int main() {
	char a[] = "  helo   test jay";
	printf("-%s-\n", a);
	rev(a);
	return 0;
}

