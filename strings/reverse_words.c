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

int main() {
    char str[] = "  this  is a test ";
    printf("'%s'\n", str);
    reversewords(str);
    printf("'%s'\n", str);
    return 0;
}


