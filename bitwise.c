#include <stdio.h>
#include <stdlib.h>
 
/***
Review of bitwise operations (bit manipulation:
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
 
/***
* some macros for bitwise operations 
 */
// align pointer to x bytes boundary
#define align(ptr, bytes) \
       ((typeof(ptr))(((uintptr_t)(ptr) + (bytes)-1) & ~((bytes)-1)))
 
// check is pointer is memory aligned to x bytes
#define isaligned(ptr, bytes) \
        (((uintptr_t)(ptr) & ~((bytes)-1)) == 0)
 
// size of variable without sizeof operator
// size_t is unsigned int
#define size(var) \
        ((size_t)(&(var)+1) - (size_t)(&(var))) 
 
// size of type without sizeof operator
#define sizetype(type) \
       ((size_t)(((type *)0) + 1)
 
// find if integer is power of 2
// power of two means only one bit is set. So if we zero it out, we get 0
#define power2(x) \
      (x != 0 && (x & (x-1) == 0)))
 
// 1010 1010b = 0xAA
// 0101 0101b = 0x55
#define swapbitsuint32(a) (((a & 0xAAAAAAAA) >> 1) | ((a & 0x55555555) << 1))
 
// 1100 1100 = 0xCC
// 0011 0011 = 0x33
#define swap2bitsuint32(a) (((a & 0xCCCCCCCC) >> 2) | ((a & 0x33333333) << 2))
 
// 1111 0000 = 0xF0
// 0000 1111 = 0x0F
#define swap4bitsuint32(a) (((a & 0xF0F0F0F0) >> 4) | ((a & 0X0F0F0F0F) << 4))
 
#define swapbytesuint32(a) (((a & 0xFF00FF00) >> 8) | ((a & 0x00FF00FF) << 8))
 
int islittleendian() {
    int t = 1;
    /* assuming int 4 bytes, it could be stored as:
            0 x 00 00 00 01: MSB in lower addr
            0 x 01 00 00 00: LSB in lower addr
                &t       &t+sizeof(int)
                 higher addr -->
       Big-endian: MSB in lower addr (the 'normal' way
                   we write (assuming addr increase left to right)
       Little-endian: MSB in higher addr (opposite of
                   what are used to in regular life)
    */
    char *ptr;
    ptr = (char *)&t; /* access lowest addr */
    return *ptr ? 1 : 0;
}
 
int numberbitssettoone(int a) {
    int cont = 0;
    while(a) {
        if (a & 1)
            cont++;
        a = a>>1;
    }
    return cont;
}
 
int numberbitssettoone_b(int a) {
    int i;
    for (i = 0; a; i++)
        a &= a-1; /* zeros out the last set bit */
 
    return i;
}
 
void inttobinary(int a, char *binary) {
    int i;
    binary += 8 * sizeof(int);
    *binary-- = '\0';
    for (i = 0; i < 8 * sizeof(int) && (char *)&a != binary; i++) {
        if (a & 1) *binary-- = '1';
        else *binary-- = '0';
        a = a>>1;
    }
}
 
/* change unsigned 4 bytes endianness */
/* Big endian: MSB first (lower address) */
unsigned int changeendianessint(unsigned int a) {
    unsigned int swapped;              // b0 b1 b2 b3
    swapped = ((a>>24) & 0xFF) |       // 0  0  0  b0 & 0x00 00 00 FF
              ((a>>8)  & 0xFF00) |     // 0  b0 b1 b2 & 0x00 00 FF 00
              ((a<<8)  & 0xFF0000) |   // b1 b2 b3 0  & 0x00 FF 00 00
              ((a<<24) & 0xFF000000);  // b3 0  0  0  & 0xFF 00 00 00
                                       // b3 b2 b1 b0
    return swapped;
}
 
/* change unsigned 1 byte endianness */
unsigned char changeendinanesschar(unsigned char a) {
    unsigned char swapped;
    swapped = (a>>8) | (a<<8);
    return swapped;
}
 
unsigned int swaporderbits(unsigned int a) {
    unsigned int swapped = 0;
    int i;
    for (i = 0; i < sizeof(int) * 8; i++) {
        if (a & 1<<i) {
            swapped |= (1<<((sizeof(int) * 8) - i - 1));
        }
    }
    return swapped;
}
 
int ispoweroftwo(int a) {
    /* ex: 4 = 100b -> 4 & 3 = 100 & 011 = 0
           5 = 101b -> 5 & 4 = 101 & 100 = 100 */
    return !((a & (a-1))==0);
}
 
int ispoweroftwo2(int a) {
    return !(a & 1);
}
 
int indexhighestbitset(unsigned int a) {
    int i;
    for (i = 0; i < sizeof(int)*8; i++) {
        if (a & 1<<i)
            return i+1;
    }
    return 0;
 
}
 
int everydigitsetone(unsigned int a) {
    return !(a & (a+1));
}
 
int maxsignedint() {
    /* logical shift: does not preserve sign
       arithmetic shift: preserves sign
       C usually uses arithmetic shift
     */
    int a = -8;
    printf("%d shifted right by one is %d\n", a, a>>1);
    if (a>>1 > 0) printf("using logical shift (does not preserve sign)\n");
    else          printf("using arithmetic shift (preserves sign)\n");
 
    // max signed number is 01111..11, which is ~0 shifted 1 to right
    // which is 2^(sizeof(int)*8-1)-1 = 2^31 - 1 = 2147483647
    int c = (~(unsigned int)0)>>1;
    return c;
}
 
int swapbits(int a, int i, int j) {
    int biti = a & 1<<i;
    int bitj = a & 1<<j;
 
    if (biti)
        a |= 1<<j; // set bit j
    else
        a &= ~(1<<j); // clear bit j
 
    if (bitj)
        a |= 1<<i; // set bit i
    else
        a &= ~(1<<i); // clear bit i
 
    return a;
}
 
int swapeverytwobits(int a) {
    return ((x & 0xAAAAAAAA)>>1 | (x & 0x55555555)<<1);
}
 
uint64_t swapnibble64bit(uint64_t var) {
 
 /* nibble is 4 bits (half byte)
 * So just mask even nibbles (var & 0x0F) and
 * shift them a nibble to the right (<<4)
 * And mask odd nibbles (var & 0xF0) and
 * shift them a nibble to the left (>>4)
 */
 return ( ((var & 0xF0F0F0F0F0F0F0F0) >>4) | ((var & 0x0F0F0F0F0F0F0F0F) <<4));
}
 
/* single line macro: #define macro(var) (expressions)
 * multi line macro:  #define macro(var) (\
 *                    {\
 *                        expressions; \
 *                    }\
 *                    )
 */
#define swapnibble64bitmacro(var) \
 ((((var) & 0xF0F0F0F0F0F0F0F0)>>4) | (((var) & 0x0F0F0F0F0F0F0F0F) <<4))
 
int main () {
    // Check if machine is little endian or big endian
    if (islittleendian())
        printf("Machine is little-endian (LSB at lowest byte)\n");
    else
        printf("Machine is big-endian (MSB at lowest byte)\n");
 
    // Get number of bits set to 1 on a int number
    int a=419;
    printf("%d has %d bits set to 1\n", a, numberbitssettoone(a));
    printf("%d has %d bits set to 1\n", a, numberbitssettoone_b(a));
 
    // print int in binary
    char binary[100] = { 0 };
    inttobinary(a, binary);
    printf("%d in binary %s\n", a, binary);
 
    // change endianess of unsigned int (should work also with positive int)
    unsigned int b = 419;
    unsigned int swapped = changeendianessint(b);
    printf("0x%x (%u) with reversed endianess is 0x%x (%u)\n", b, b, swapped, swapped);
 
    // swap order of bits of unsigned int
    swapped = swaporderbits(b);
    inttobinary(swapped, binary);
    printf("%u with bits in opposite order is %u (%s)\n", b, swapped, binary);
 
    // check if number is power of two
    a = 88;
    if (ispoweroftwo(a))
        printf("%d is a power of two\n", a);
    else
        printf("%d is not a power of two\n", a);
 
    // find index highest bit set
    printf("index highest bit set of %d is %d\n", a, indexhighestbitset(a));
 
    // determine if every digit in binary representation is 1
    if (everydigitsetone(a))
        printf("every digit is set to 1 in %d\n", a);
    else
        printf("not every digit is set to 1 in %d\n", a);
 
   // print the max signed integer value with bitwise operations
   printf("Max signed integer: %d\n", maxsignedint());
 
   // swap bits in number
   printf("%d with bits 0 and 3 swapped: %d\n", a, swapbits(a, 0, 3));
 
   // swap every two bits: 10 01 11 00 -> 01 10 11 00
   int d =  swapeverytwobits(a);
 
   // swap nibbles
   uint64_t a = 0x1233456789; /* uint64_t is unsigned long long -> %llx */
   printf("0x%llx, nibbles swapped 0x%llx\n", a, swapnibble64bit(a));
   printf("0x%llx, nibbles swapped 0x%llx\n", a, swapnibble64bitmacro(a));
   return 0;
}
