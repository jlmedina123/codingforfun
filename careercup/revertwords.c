
/*
Code a function that receives a string composed by words separated by spaces and returns a string where words appear in the same order but than the original string, but every word is inverted. 
Example, for this input string

@"the boy ran"
the output would be

@"eht yob nar"
Tell the complexity of the solution.
*/

#include <stdio.h>
#include <stdlib.h>

void reverseword(char *str, char * l, char * r) {
	while (l < r) {
		*l ^= *r;
		*r ^= *l;
		*l ^= *r;
		l++;
		r--;
	}
}
char * reverse(char *str) {
	char *c = str;
	int l = 0;
	int r = 1;
	while (str[r]) {
		while (str[r] && (str[r] != ' ')) {
			r++;
		}	
		reverseword(str, &str[l], &str[r-1]);
		l = r+1;
		r = l;
	}
	return str;
}

int main(int argc, char *argv[]) {
	char test[] = "test1 test2 hello";
	printf("%s\n", test);
	reverse(test);
	printf("%s\n", test);
	return 0;
}	
