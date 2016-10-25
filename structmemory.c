#include <stdio.h>
#include <stdlib.h>
 
int main() {
 
    /* integral types */
    printf("int: bytes %d, min: %d, max: %d\n",
           sizeof(int), -(2^(sizeof(int)*8-1)), (2^(sizeof(int)*8-1)-1));
    printf("short: bytes %d\n", sizeof(short));
    printf("long: bytes %d\n", sizeof(long));
    printf("long long: bytes %d\n", sizeof(long long));
 
    /* floating types */
    printf("float: bytes %d(1 byte exponent, 3 bytes mantissa)\n", sizeof(float));
    printf("double: bytes %d (11 bits exponent, 53 bits mantissa)\n", sizeof(double));
    printf("long double: bytes %d (16 bits exponent, 64 bits mantissa)\n", sizeof(long double));
 
 
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
    unsigned int * ptr = &test1;
    test1.a = 0x11223344;
    test1.b = 0x1122334455667788;
    test1.c[0] = 0x11;
    test1.c[1] = 0x22;
    test1.c[2] = 0x33;
    test1.c[3] = 0x44;
    test1.e = -12;
    test1.f = 0x1122334455;
    test1.g = 0x01;
    int i;
    printf("sizeof struct: %d\n", sizeof(struct mystruct)); // packed would be 32 bytes
    for (i = 0; i*sizeof(int) < sizeof(struct mystruct); i++) {
        printf("ptr addr %p: 0x%x\n", ptr, *ptr);
        ptr++;
    }
    return 0;
}
 
 
/*
	int: bytes 4, min: -29, max: 28
	short: bytes 2
	long: bytes 8
	long long: bytes 8
	float: bytes 4(1 byte exponent, 3 bytes mantissa)
	double: bytes 8 (11 bits exponent, 53 bits mantissa)
	long double: bytes 16 (16 bits exponent, 64 bits mantissa)
	sizeof struct1: 16
	sizeof struct: 40
	ptr addr 0x7fffc2ae1270: 0x11223344
	ptr addr 0x7fffc2ae1274: 0x0
	ptr addr 0x7fffc2ae1278: 0x55667788
	ptr addr 0x7fffc2ae127c: 0x11223344
	ptr addr 0x7fffc2ae1280: 0x44332211
	ptr addr 0x7fffc2ae1284: 0x0
	ptr addr 0x7fffc2ae1288: 0xfffffff4
	ptr addr 0x7fffc2ae128c: 0x22334455
	ptr addr 0x7fffc2ae1290: 0x1
	ptr addr 0x7fffc2ae1294: 0
*/
