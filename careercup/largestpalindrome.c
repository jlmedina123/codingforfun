
/*
Imagine we have a large string like this
"ABCBAHELLOHOWRACECARAREYOUIAMAIDOINGGOOD" which contains
multiple palindromes within it, like ABCBA, RACECAR, ARA, IAMAI etc...
 Now write a method which will accept this large string and return
the largest palindrome from this string. If there are two palindromes
 which are of same size, it would be sufficient to just return any one
of them.

https://www.careercup.com/question?id=245679
*/
char * largestpalindrome(char *str) {
	char *p = str;
	char *t;
	for (i = 0; i < lenstr; i++) {
		j = i;
		while (i-j > 0 && i + j < lenstr && str[i-j] == str[i+j]) {
			j++;
			if (j > jmax) {
				jmax = j;
				palmax = &str[i];
			}
		}
	}
	return palmax;
		
