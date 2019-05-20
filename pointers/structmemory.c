#include <stdio.h>
#include <stdlib.h>
 
int main() {
 
    /* integral types */
    printf("int: bytes %lu, min: %lu, max: %lu\n",
           sizeof(int), -(2^(sizeof(int)*8-1)), (2^(sizeof(int)*8-1)-1));
    printf("short: bytes %lu\n", sizeof(short));
    printf("long: bytes %lu\n", sizeof(long));
    printf("long long: bytes %lu\n", sizeof(long long));
 
    /* floating types */
    printf("float: bytes %lu(1 byte exponent, 3 bytes mantissa)\n", sizeof(float));
    printf("double: bytes %lu (11 bits exponent, 53 bits mantissa)\n", sizeof(double));
    printf("long double: bytes %lu (16 bits exponent, 64 bits mantissa)\n", sizeof(long double));
 
 
    /* structure with different data types */
    struct mystruct {
        unsigned int a;
        unsigned long b;
        char c[4];
        unsigned int d;
        int e;
        unsigned int f;
        unsigned int g;
    } test1;
    
    unsigned int * ptr = (unsigned int *)&test1;

    test1.a = 0x11223344;
    // padding so next field is aligned to its byte boundary (8)
    test1.b = 0x1122334455667788;
    test1.c[0] = 0x11; // &c
    test1.c[1] = 0x22;
    test1.c[2] = 0x33;
    test1.c[3] = 0x44; // &c+4
    // padding again?
    // memory (-->higher addr)
    // 0x11 22 33 44
    // Big endian would read    11 22 33 44
    // Little endian would read 44 33 22 11
    test1.e = -12;
    // 2's complement: 00000000 00000000 00000000 00001100 binary
    //                 11111111 11111111 11111111 11110011 invert
    //                 11111111 11111111 11111111 11110100 add 1
    //                 0xFF     0xFF     0xFF     0xF 4
    test1.f = 0x1122334455;
    // warning: implicit conversion from
    //  'long' to 'unsigned int' changes value from
    //  73588229205 to 573785173 [-Wconstant-conversion]
    // 0x 55 44 33 22 in little endian
    // 0x 11 22 33 44 in big endian???
    test1.g = 0x06;
    // Big endian: 0x 00 00 00 06
    // Little end: 0x 06 00 00 00

    int i;
    printf("sizeof struct: %lu\n", sizeof(struct mystruct)); // packed would be 32 bytes
    
    printf("printing 4 bytes at a time:\n");
    for (i = 0; i*sizeof(int) < sizeof(struct mystruct); i++) {
        printf(" *%p: 0x%x\n", ptr, *ptr);
        ptr++;
    }

    printf("printing 1 byte at a time:\n");
    unsigned char *p;
    printf("test1: %x, end: %x\n", &test1, &test1 + sizeof(struct mystruct));
    // next line is forever loop
    //for (p = (char *)&test1; (uintptr_t)p < (uintptr_t)(&test1 + sizeof(struct mystruct)); p+=4) {
    for (p = (uint8_t *)&test1; p < ((uint8_t *)&test1 + sizeof(struct mystruct)); p+=4) { 
        printf("addrs: %p-%p: 0x%x 0x%x 0x%x 0x%x\n", p, p+4, *p, *(p+1), *(p+2), *(p+3));
    }

    return 0;
}
 
/*
 stdout from my mac with gcc:

int: bytes 4, min: 18446744073709551587, max: 28
short: bytes 2
long: bytes 8
long long: bytes 8
float: bytes 4(1 byte exponent, 3 bytes mantissa)
double: bytes 8 (11 bits exponent, 53 bits mantissa)
long double: bytes 16 (16 bits exponent, 64 bits mantissa)
sizeof struct: 40
printing 4 bytes at a time:
 *0x7fff5a84faf0: 0x11223344
 *0x7fff5a84faf4: 0x0
 *0x7fff5a84faf8: 0x55667788
 *0x7fff5a84fafc: 0x11223344
 *0x7fff5a84fb00: 0x44332211
 *0x7fff5a84fb04: 0x0
 *0x7fff5a84fb08: 0xfffffff4
 *0x7fff5a84fb0c: 0x22334455
 *0x7fff5a84fb10: 0x6
 *0x7fff5a84fb14: 0x7fff
printing 1 byte at a time:
test1: 5a84faf0, end: 5a850130
addrs: 0x7fff5a84faf0-0x7fff5a84faf4: 0x44 0x33 0x22 0x11
addrs: 0x7fff5a84faf4-0x7fff5a84faf8: 0x0 0x0 0x0 0x0
addrs: 0x7fff5a84faf8-0x7fff5a84fafc: 0x88 0x77 0x66 0x55
addrs: 0x7fff5a84fafc-0x7fff5a84fb00: 0x44 0x33 0x22 0x11
addrs: 0x7fff5a84fb00-0x7fff5a84fb04: 0x11 0x22 0x33 0x44
addrs: 0x7fff5a84fb04-0x7fff5a84fb08: 0x0 0x0 0x0 0x0
addrs: 0x7fff5a84fb08-0x7fff5a84fb0c: 0xf4 0xff 0xff 0xff
addrs: 0x7fff5a84fb0c-0x7fff5a84fb10: 0x55 0x44 0x33 0x22
addrs: 0x7fff5a84fb10-0x7fff5a84fb14: 0x6 0x0 0x0 0x0
addrs: 0x7fff5a84fb14-0x7fff5a84fb18: 0xff 0x7f 0x0 0x0
 
*/
                       


