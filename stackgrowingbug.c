#include <stdio.h>

int main() {
    int i;
    int a[10];
    int b;
    printf("&i %p\n&b %p\n", &i, &b);
    for (i=0; i<=11; i++) {
        printf("i %d, &i %p, &a[i] %p\n", i, &i, &a[i]);
        a[i] = 0;
    }
}

/*
 * Linux:
 *   stack:
 *      highest addr:  &i          
 *                     free  (highest addr - 4)
 *                     free  (highest addr - 8)
 *                     &a[9] (highest addr - 16)
 *                     ...
 *                     &a[0]
 *                     &b
 *
 * &i 0x7fff759886bc
 * &b 0x7fff7598868c
 * i 0, &i 0x7fff759886bc, &a[i] 0x7fff75988690
 * i 1, &i 0x7fff759886bc, &a[i] 0x7fff75988694
 * i 2, &i 0x7fff759886bc, &a[i] 0x7fff75988698
 * i 3, &i 0x7fff759886bc, &a[i] 0x7fff7598869c
 * i 4, &i 0x7fff759886bc, &a[i] 0x7fff759886a0
 * i 5, &i 0x7fff759886bc, &a[i] 0x7fff759886a4
 * i 6, &i 0x7fff759886bc, &a[i] 0x7fff759886a8
 * i 7, &i 0x7fff759886bc, &a[i] 0x7fff759886ac
 * i 8, &i 0x7fff759886bc, &a[i] 0x7fff759886b0
 * i 9, &i 0x7fff759886bc, &a[i] 0x7fff759886b4
 * i 10, &i 0x7fff759886bc, &a[i] 0x7fff759886b8
 * i 11, &i 0x7fff759886bc, &a[i] 0x7fff759886bc
 * i 1, &i 0x7fff759886bc, &a[i] 0x7fff75988694
 * i 2, &i 0x7fff759886bc, &a[i] 0x7fff75988698
 * i 3, &i 0x7fff759886bc, &a[i] 0x7fff7598869c
 * ... infinite loop
 * 
 * Mac:
 *          Stack:       
 *                     &a[11]
 *                     &a[
 *                     &i
 *                     &b
 *
 *
 * &i 0x7fff5152a9f8
 * &b 0x7fff5152a9f4
 * i 0, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa00
 * i 1, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa04
 * i 2, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa08
 * i 3, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa0c
 * i 4, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa10
 * i 5, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa14
 * i 6, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa18
 * i 7, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa1c
 * i 8, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa20
 * i 9, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa24
 * i 10, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa28
 * i 11, &i 0x7fff5152a9f8, &a[i] 0x7fff5152aa2c
 * Abort trap: 6
 */

