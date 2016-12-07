#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void reversestring(char *ptra, char *ptrb) {
    assert(ptrb >= ptra);
    while(ptrb > ptra) {
        assert(ptra && ptrb); 
        *ptra ^= *ptrb;
        *ptrb ^= *ptra;
        *ptra ^= *ptrb;
        ptra++;
        ptrb--;
    }
}
void reversewords(char *str) {
    assert(str);
    char *ptra = str;
    char *ptrb = str;
    // skip first whitespaces
    while (ptra && *ptra && *ptra == ' ')
        ptra++;
    ptrb = ptra;

    while (ptra && *ptra != '\0') {
        while (*ptrb != '\0' && *ptrb != ' ') 
            ptrb++;
        reversestring(ptra, ptrb-1);
    
        while (*ptrb != '\0' && *ptrb == ' ') 
            ptrb++;
           
        ptra = ptrb;
    }
    reversestring(str, --ptrb);
}


/* 
 * find if all characters unique
 */

#define FALSE 0
#define TRUE 1
 
// time O(n^2). No extra space needed
int uniqueCharac1(char *ptr1) {
    char *ptr2;
    while (*ptr1++ != '\0') {
        ptr2 = ptr1;
        while (*ptr2++ != '\0') {
            if (*ptr2 == *ptr1) {
                printf("Letter %c repeated\n", *ptr2);
                return FALSE;
            }
        }
    }
    return TRUE;
}
 
// time O(n). Extra space O(1). Using bitmap
int uniqueCharac2(char *ptr) {
    unsigned int bit_field = 0x0; // needs 27 bits (one bit per letter).
    // int gives 4 bytes (32 bits)
    int character;
    while (*ptr++ != '\0') {
        character = *ptr - 'a' + 1; // 'a' is 1
        //printf("Letter %c is number %d\n", *ptr, character);
        // checking if bit set
        if (bit_field && (1<<character)) {
            printf("Letter %c repeated\n", *ptr);
            return FALSE;
        }
        // setting bit
        bit_field |= (1 << character);
    }
    return TRUE;
}
 
char * quicksort(char *string) { // TO DO
    return string;
}
int binarysearch(char *string, char letter) { // TO DO
    return TRUE;
}
 
// time O(n*log n). No extra space needed. Using quicksort, and binary search after
int uniqueCharac3(char *ptr) {
    char *sorted = quicksort(ptr);
    while (*ptr++ != '\0') {
        if (binarysearch(ptr + 1, *ptr)) {
            printf("Letter %c repeated\n", *ptr);
            return TRUE;
        }
    }
    return FALSE;
}


int hascharacter(const char *str, const size_t n, const char c) {
    int i;
    for (i = 0; i < n; i++) 
        if (str[i] == c)
            return 1;
    return 0;
}


// remove characters from string without memmov 
void removechars1(char *str, char *remove, size_t n) {
    char *dest;
    char *src;
    //quicksort(remove, n);
    for (dest = src = str; *src != '\0'; src++) {
        *dest = *src;
        if (!hascharacter(remove, n, *src)) // if(!binarysearch(removeordered, n, *src))
            dest++;
    }
    // dont forget to terminate string!!
    *dest = '\0';
}    

int main() {

    /* reverse the order of words within a string */
    char str[] = "  this  is a test ";
    printf("'%s'\n", str);
    reversewords(str);
    printf("'%s'\n", str);


    /* find the first non-repeated character */
  	// use hash or 64 bit mask 

	/* find if all characters are unique */
    char string[] = "asdfqwersdfga";
    printf("Characters in %s are %s unique\n", string, uniqueCharac1(string)? " " : "not"); 
    printf("Characters in %s are %s unique\n", string, uniqueCharac2(string)? " " : "not");  

	/* remove characters from string */
    char str2[] = "this is a test";
    char remove[] = {'a', 'b', 's'};
    printf("String: '%s'\n", str2);
    removechars1(str2, remove, 3);
    printf("String without '%s': '%s'\n", remove, str2);


    return 0;
}


