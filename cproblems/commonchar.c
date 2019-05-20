
#include <stdio.h>
#include <stdlib.h>

/* Write a function f(a, b) which takes two character string 
arguments and returns a string containing only the characters
 found in both strings in the order of a. Write a version which
  is order N-squared and one which is order N.
*/


int getlenght1(const char s[]) { // same as '(const char * const s)'
    int k = 0;
    while(s[k++]); // same as      while(s[k++] != '\0'); 
                   // and same as  while (*(s + k++) != '\0')
    k--;
    printf("size %d\n", k);
    return k;
}

int getlenght2(const char * s) {
    const char * s0 = s;
    while (*s++); // wouldnt work if s defined as const char * const s
    printf("size %s: %d\n", s0, (s-s0-1));
    return (s-s0-1);
}

char * solution_nsquare(const char * const a, const char * const b) {
    char *common;
    int i, j, k;
    int na = getlenght2(a);
    int nb = getlenght2(b);
    common = malloc(sizeof(*common) * na);
    k = 0;
    for (i = 0; i < na; i++) {
        for (j = 0; j < nb; j++) {
            if (b[j] == a[i]) {
                common[k] = a[i];
                k++;
            }
        }
    }
    common[k] = '\0';
    return common;
}

/* Review of bitwise operations (bit manipulation:
    && is logical operator: result is true (1) or false (0)
    & is bitwise operator, applies && to each bit. result is a number
    set bit x:              vble |= (1<<x)
    set bits x and y:       vble |= (1<<x|1<<y)
    clear bit x:            vble &= ~(1<<x)
    toggle (change) bit x:  vble ^= (1<<x)
    check if bit x set:     if(vble & (1<<x))
    get lower x bits:       v = vble & (2**x-1)  
    get higher x bits:      v = vble & ~(2**(16-x)-1)
 */
char * solution_linear(const char a[], const char b[]) {
    int i, letter, k;
    unsigned long bitarray = 0x0;
    i = 0;
    /* scan b string */
    while (b[i]) {
        letter = b[i] - 'a' + 1;
        bitarray |= 0x1<<letter; /* set bit for that letter */
        i++;
    }
    /* now scan a, so common letters are saved in same order as a */
    char *common = malloc(sizeof(char) * i);
    i = 0; k = 0;
    while (a[i]) {
        letter = a[i] - 'a' + 1; // 'x' for characters, "x" for terminated strings
        if (bitarray & (0x1<<letter)) { /* check if bit set */
            common[k++] = a[i];
        }
        i++;
    }
    common[k] = '\0';
    
    /* 'common' was allocated dynamically, so it won't be erased when function
        returns. But it needs to be freed in main */
    return common;
}
    
int main() {
    char *a = "asdfqwer";
    char b[] = "skelrpfa";
    char *common1 = solution_nsquare(a, b);
    char *common2 = solution_linear(a, b);
    printf("a: %s\nb: %s\ncommon square: %s\ncommon linear: %s\n",
            a, b, common1, common2);
    free(common1);
    free(common2);
    return 0;
}     
/*
gcc commonchar.c
./a.out 
size asdfqwer: 8
size skelrpfa: 8
a: asdfqwer
b: skelrpfa
common square: asfer
common linear: asfer
*/
