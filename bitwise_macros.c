#include <stdio.h>
#include <stdlib.h>
 
/*** Review of bitwise operations
    && is logical operator: result is true (1) or false (0)
    & is bitwise operator, applies && to each bit. result is a number
    set bit x:              vble |= (1<<x)
    set bits x and y:       vble |= (1<<x|1<<y)
    clear bit x:            vble &= ~(1<<x)
    toggle (change) bit x:  vble ^= (1<<x)
    check if bit x set:     if(vble & (1<<x))
    get lower x bits:       v = vble & (2**x-1)
    get higher x bits:      v = vble & ~(2**(16-x)-1)
***/

/* Review of macros:
 single line macro: #define macro(var) (expressions)
 multi line macro:  #define macro(var) (\
                     {\
                         expressions; \
                     }\
                     )
  macro to return value: macro needs to be one expression

*/

/*
https://gcc.gnu.org/onlinedocs/gcc-5.1.0/cpp/Macros.html

 - object-like macro: give symbolic name to numeric constants
 - function-like macro: put parenthesis after macro name
 - stringfication: convert macro argument into string constant. parameter with leading # is replaced
    with literal text of actual argument
 - token pasting: two tokes on both sides of ## are combined into single token
 - variadic macro: accepts variable number of arguments. All tokens in argument list after last named ar
   argument become variable argument. The sequence of tokens replace identifier __VAR_ARGS__ in the 
   macro body (or the name before the ..., such as myargs...)

#define eprintf1(...) 		 fprintf(stderr, __VA_ARGS__)
#define eprintf2(myargs...)	 fprintf(stderr, myargs)
#define eprintf3(format, ...) fprintf(stderr, format, __VAR_ARGS__) // needs at least one input variable
	eprintf3("hello", ); // wrong
	eprintf3("hello");   // correct, expands to fprintf(stderr, "hello", );

#define eprintf4(format, ...) fprintf(stderr, format, ##__VAR_ARGS__) // removes comma if no var_args
	eprintf4("hello");    // correct, expands to fprintf(stderr, "hello");


 - predefined macros
	+ standards: __FILE__, __LINE__, __DATE__, __TIME__, __STDC__, _STDC_VERSION__
    + common: __COUNTER__, __GNUC__, __INT_MAX__, __BYTE_ORDER__, __ORDER_LITTLE_ENDIAN__, __ORDER_BIG_ENDIAN__
	+ system specific: 
 - undefining macros: #undef FOO

macro pitfalls
 - each macro argument should be sorrounded by paranthesis
 - full macro in parenthesis as well
 - macro with compound statments #define FOO(a, b) { a = ...; ...;} is called without ; 
   To call it like function with ; add #define FOO(a, b) do { a= ...; ...;} while (0) or ({ a=;...;})
 - #define min(x, y) ({ typeof(X) x_ = (x); ... ; })   call as int a = min(1, 3); 


*/


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__


#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

// example of stringfication and token pasting
struct command {
       char *name;
       void (*function) (void);
};
struct command commands[] = {
       { "quit", quit_command },
       { "help", help_command },
       ...
};

// with macro
#define COMMAND(NAME)  { #NAME, NAME ## _command }
struct command commands[] = {
       COMMAND (quit),
       COMMAND (help),
       ...
};

#ifdef DEBUG
#define printt(fmt, ...) \
    do { \
        struct timeb tp; \
        ftime(&tp); \
        printf("%ld.%d: " fmt, tp.time, tp.millitm, ##__VA_ARGS__); \
    } while (0)
#else
#define printt(...) while(0){}
#endif

/***
 * some macros for bitwise operations 
 */
#define setbit(v, i) 		(v |= (1<<i))   // LSbit is bit 0
#define clearbit(v, i) 		(v &= ~(1<<i))  // LSbit is bit 0
#define togglebit(v, i) 	(v ^= (1<<i))   // LSbit is bit 0
#define isbitset(v, i)  	(v & (1<<i))    // LSbit is bit 0
#define updatebit(v, i, new)    (v = new ? v | (1<<i) : v & ~(1<<i))
#define updatebit2(v, i, new)   (v = ((v & ~(1<<i)) | new<<i)) 

#define getLSbitstoi(v, i)  	(v & ((1<<(i+1))-1))     // get least-significat bits from 0 to i (inclusive)
#define getMSbitsfromi(v, i)  	(v & ~((1<<(i+1))-1))    // get most-significant bits from sizeof(v)*8 to i (inclusive)

#define clearLSbitstoi(v, i)    (v &= ~((1<<(i+1))-1))   // clear least-significat bits from 0 to i (inclusive) (i+1 bits cleared)
#define clearMSbitsfromi(v, i)  (v &= ((1<<(i+1))-1))      // clear most-significat bits from sizeof(v)*8 to i (not inclusive, i not cleared)

#define setLSbitstoi(v, i)	(v |= ((1<<(i+1))-1))    // set least-significant bits from 0 to i (inclusive)
#define setMSbitsfromi(v, i)	(v |= ~((1<<(i+1))-1))   // set least-significant bits from N to i (not inclusive, bit i not set)

#define setbitsbetweenij(v, i, j)   (v |= (((1<<(i-j+1))-1)<<j))   // i > j (both inclusive, both set, LSbit=0)
#define clearbitsbetweenij(v, i, j) (v &= ~(((1<<(i-j+1))-1)<<j))  // i > j (both inclusive, both cleared, LSbit=0)



// align pointer to x bytes boundary
#define align(ptr, bytes) \
       ((typeof(ptr))(((uintptr_t)(ptr) + (bytes)-1) & ~((bytes)-1)))
/* aligning to 16 byte boundary
 *    ptr = (ptr + 15) & ~15 
 *    0x0F = 0000 1111
 *    ptr & ~ 0x0F resets the last 4 bits
 *    if ptr = 1011 1101 -> ptr & 1111 0000 = 1011 0000
 *    2 byte boundary -> reset 1 bit, ..., 16 byte boundary -> reset 4 bits, 32 byte boundary -> reset 8 bits
 */

// check is pointer is memory aligned to x bytes
#define isaligned(ptr, bytes) \
        (((uintptr_t)(ptr) % (bytes)) == 0)
 
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

#define changeendiannesuint32(a) (\
                ((a & 0xFF) << 24) |\
                ((a & 0xFF00) << 8) |\
                ((a & 0xFF0000) >> 8) |\
                ((a & 0xFF000000) >> 24))



#define toupper(c) ((c) >= ’a’ && (c) <= ’z’? (c) + ’A’ - ’a’: (c))
#define tolower(c) ((c) >= ’A’ && (c) <= ’Z’? (c) + ’a’ - ’A’: (c))


uint32_t merge_m_into_n(uint32_t m, uint32_t n, int i, int j) {
    // 1) clear bits from i to j in n
    uint32_t mask = ((1<<(i-j+1))-1)<<j;
    uint32_t n_clear = n | mask;

    // 2) shift m (and make sure other bits cleared)
    m &= ((1<<(i-j+1))-1);  // clear MS bits
    m = m << j;

    // 3) merge
    return m | n_clear;
}
                         // clear m bits i:j           clear n outside of bits i:j            
#define merge(m, n, i, j) ((m & ~(((1<<i-j+1)-1)<<j))  | (n & ((1<<i-j+1)-1)<<j))
    
  
int islittleendian() {
    int t = 1;
    /* assuming int 4 bytes, it could be stored as:
     *  
            0 x 00 00 00 01: MSB in lower addr
            0 x 01 00 00 00: LSB in lower addr
                &t       &t+4bytes
                ptr
                higher addr -->
                 
       Big-endian: MSB in lower addr (the 'normal' way
                   we write (assuming addr increase left to right)
       Little-endian: MSB in higher addr (opposite of
                   what are used to in regular life)
       typecasting pointer to variable always gets lowest address       
       For Big-endian, pointer gets MSB (lowest addr)
       For Little-endian, pointer gets LSB (lowest addr)     
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
/*unsigned char changeendinanesschar(unsigned char a) {
    unsigned char swapped;
    swapped = (a>>8) | (a<<8);
    return swapped;
}*/
 
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
   // if power of two, only one bit 1, all other zeros
   // so when 1 bit is zeroed out (a &= (a-1)) 
    /* ex: 4 = 100b -> 4 & 3 = 100 & 011 = 0
           5 = 101b -> 5 & 4 = 101 & 100 = 100 */
    return !((a & (a-1))==0);
}
 
int iseven(int a) {  // ??
    return !(a & 1);
}
 
int indexhighestbitset(unsigned int a) {
    int i;
    // LSbit = 0
    // return -1 if no bit set
    int highest=-1; 
    for (i = 0; i < sizeof(int)*8; i++) {
        if (a & 1<<i)
            highest = i;
    }
    return highest;
 
}

int indexlowestbitset(unsigned int a) {
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
 
int swapeverytwobits(int x) {
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
 *
 * macro to return value: macro needs to be one expression
 */
#define swapnibble64bitmacro(var) \
 ((((var) & 0xF0F0F0F0F0F0F0F0)>>4) | (((var) & 0x0F0F0F0F0F0F0F0F) <<4))
 



/* fixed-point math:
 * http://stackoverflow.com/questions/10067510/fixed-point-arithmetic-in-c-programming
 *
 * Stores values multiplied by a certain amount. 
 * Uses integer arithmetic but still representing fractions 
 */

// Assuming uint32
#define SHIFT 16 // integer 16 bits, fraction 16 bits
#define SHIFT_MASK ((1 << SHIFT) - 1)
#define store(value) (value << SHIFT)        // eg: uint32 a = store(5);
// for +, -: need to use store: a += store(10);
// for *, /: no need to shift: a /= 2;
#define retrieve(value) (value >> SHIFT)    // eg: printf("%d", retrieve(a))
#define fractiondouble(value) ((double)(value & SHIFT_MASK) / (1 << SHIFT))
// eg: printf("%f", fractiondouble(a));




int main () {

    // testing bitwise macros
    uint32_t v;
    v = 0;
    printf("setbit(0, 4): 0x%x\n", setbit(v, 4));
    v = 0xFFFFFFFF;
    printf("clearbit(0xFFFFFFFF, 4): 0x%x\n", clearbit(v, 4));
    v = 0xFFFFFFFF;
    printf("togglebit(0xFFFFFFFF, 4): 0x%x\n", togglebit(v, 4));
    v = 0;
    printf("isbitset(0, 4): %s\n", isbitset(v, 4) ? "set" : "clear");
    printf("updatebit(0, 4, 1): 0x%x\n", updatebit(v, 4, 1));
    v = 0;
    printf("updatebit2(0, 4, 1): 0x%x\n", updatebit2(v, 4, 1));
    v = ~0;
    printf("getLSbitstoi(0xFFFFFFFF, 8): 0x%x\n", getLSbitstoi(v, 8));
    printf("getMSbitsfromi(0xFFFFFFFF, 8): 0x%x\n", getMSbitsfromi(v, 8));
    printf("clearLSbitstoi(0xFFFFFFFF, 8): 0x%x\n", clearLSbitstoi(v, 8));
    v = ~0;
    printf("clearMSbitsfromi(0xFFFFFFFF, 8): 0x%x\n", clearMSbitsfromi(v, 8));
    v = 0;
    printf("setLSbitstoi(0, 8): 0x%x\n", setLSbitstoi(v, 8));
    v = 0;
    printf("setMSbitsfromi(0,8): 0x%x\n", setMSbitsfromi(v, 8));
    v = 0;
    printf("setbitsbetweenij(0, 8, 4): 0x%x\n", setbitsbetweenij(v, 8, 4));
    v = ~0;
    printf("setbitsbetweenij(~0, 8, 4): 0x%x\n", clearbitsbetweenij(v, 8, 4));

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
    printf("0x%x (%u) with reversed endianess is 0x%x (%u)\n", b, b, 
            changeendiannesuint32(b), changeendiannesuint32(b));

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
   uint64_t c = 0x1233456789; /* uint64_t is unsigned long long -> %llx */
   printf("0x%llx, nibbles swapped 0x%llx\n", c, swapnibble64bit(c));
   printf("0x%llx, nibbles swapped 0x%llx\n", c, swapnibble64bitmacro(c));
   return 0;
}
