/*
 * C pitfalls
 */

y = x/*p       /* sets y to the value of x */;
y = x / *p;    /* divides x over value pointeed by p */
y = x /(*p);   /* divides x over value pointeed by p */
a=-1;   /* a = a - 1 for old compiler, a = -1 for newer compilers */
a=/*b;  /* a = a/(*b) for some old compilers, comment for newer ones */
p - > a;         /* illegal */
 
printf('\n');    /* fails at run-time */

char *a = 'yes'; /* 'yes' is integer composed of values 'y', 'e', 's',
                    typecasted to char */

char *a = "yes"; /* string with three characters and null terminated,
                    in read-only data memory. Cannot change */
char a[] = "yes"; /* string in read-only data memory. Copied to stack,
                   so it can change */


int *d = {1, 2,3, 5}; /* warning: excess elements in scalar initializer 
                         warning: incompatible integer to pointer conversion 
                                  initializing 'int *' with an expression of type 'int'
                        */
d[3] = 10;            /* segmentation fault */


int e[] = {1, 2, 3, 4}; 
e[3] = 10;              /* works fine */


if (flags & FLAG != 0) /* equivalent to if (flags & (FLAG !=0)) */
r = h<<4 + l;    /* equivalent to r = h << (4 + l); */
                 /* operator precedence:
                        1: func call (),
                        2: unary operators (++, --,
                        3: binary operators (<<, &&, =
                        4: ternary operator ( cond ? a: b )
                        4: comma
                 */
if (a < b == c < d) /* equivalent to (a<b) == (c<d) */ 

while (c=getc(in) != EOF) /* assignment lower preference than comparison 
                             so equivalent to c = (getc(in) != EOF),
                            which is true or false */ 

if (x[i] > big); /* equivalent to if (x[i] > big) { }, which is { } */
 
struct foo {
  int x;
}
f() {
 ... }          /* f returns type struct foo */
 
func();   /* calls function */
func;     /* evaluates address of function */
 
i = 0;
while (i < n)
    y[i] = x[i++];  /* wrong (maybe): no guarantee that y[i] is
                       evaluated before i incremented. */
 
i = 0;
while (i < n)
    y[i++] = x[i];  /* wrong (maybe): same as previous */
 
i = 0;
while (i < n) {
    y[i] = x[i];
    i++;
}                   /* works fine */
 

for (i = 0; i < n; i++)     
    y[i] = x[i];  /* works fine */ 
                  /* bitwise operators: &, |, ~     -> arguments per bit
                     logical operators: &&, ||, !  -> arguments true or false
                   */
 
/* it works, but shouldn't be used. It should be && */
i = 0;
while (i < tabsize & tab[i] != x)
    i++;
 
/* if big-endian (MSB to lower addr), a[10] is actually i,
   so this turns out to be an infinite loop */
int i, a[10];
for (i=1; i<=10; i++)
    a[i] = 0;
 
/* fails because '2' is an integer. It should be '2.0'
   conversion rules:
      - integer values shorter than an int are converted to int
      - floating-point values shorter than a double are converted to double.
      - All other values are left unconverted.
   Actually, using the <math.h> this would work. It looks like the
   library function can checks for the type and typecasts automatically
*/
double sqrt(double a);
double s;
s = sqrt (2);
printf ("%g\n", s);
 
/* there is no integer to float conversion. Typecasting is required
   But if one of the numbers in the operation is float, then the result
   of the operation is float. */
int main() {
    int a = 3/2;
    printf("%d\n", a);     /* 1 */
    float f = 3/2;
    printf("%f\n", f);     /* 1.000000 */
    float g = 3.0/2.0;
    printf("%f\n", g);     /* 1.500000 The same if 3.0/2 or 3/2.0 */
    float h = (float)(3/2);
    printf("%f\n", h);     /* 1.000000 */
    float j = (float)3/2;
    printf("%f\n", j);     /* 1.500000 */
    float k = 3/(float)2;
    printf("%f\n", k);     /* 1.500000 */
    return 0;
}
 
/* scanf expects pointer to integer. This might work for some smart compilers,
   For some compiler, if given 5 values, output is 0 0 0 0 0 1 2 3 4
   Storing c in char might overwrite i in big endian. So every time a value is
   entered, it resets i
 */
main() {
  int i;
  char c;
  for (i=0; i<5; i++) {
    scanf ("%d", &c);
    printf ("%d ", i);
  }
  printf ("\n");
}
 
/* fails because r doesnt have memory allocated */
char *r;
strcpy (r, s);
strcat (r, t);
 
/* might fail if r is not big enough */
char r[100];
strcpy (r, s);
strcat (r, t);
 
/* fails because it r needs one more space for null termination
   and it doesnt check if malloc worked */
char *r;
r = malloc (strlen(s) + strlen(t));
strcpy (r, s);
strcat (r, t);
 
/* should work */
char *r;
r = malloc (strlen(s) + strlen(t) + 1);
if (!r)
    exit (1);
strcpy (r, s);
strcat (r, t);
 
int main() {
    char *p, *q;
    p = "xyz";
    q = p;
    q[1] = "Y";  // compile-time error: assigning addr to char type
    q = 'Y';     // run-time error: Bus error 10 (cant change string
                 // literals, they are read-only mem
                 // commenting previous two lines works
    printf("%s\n", p);
    printf("%s\n", q);
    return 0;
}
 
char *p = NULL;
if (p == (char *) 0)  // valid
if (strcmp (p, (char *) 0) == 0) // invalid, it deferences null pointer
printf (p);           // invalid
printf ("%s", p);   // invalid 
 
/* unsigned overflow wraps around
   signed overflow is undefined
   Wrong way to check if two positive signed integers overflow when summed
 */
int a, b;
if (a + b < 0)
    overflow();
 
/* Right way to check overflow of two positive integers */
if ((int) ((unsigned) a + (unsigned) b) < 0)
    overflow();
 
/* wrong undefined behavior. getchar returns int, because it needs to
   be able to return any char, plus EOF. So, depending on the compiler,
   it may never be able to return EOF if return typecasted to EOF
 */
char c;
while ((c = getchar()) != EOF)
    putchar (c);
 
/* parenthesis in macro protect in case an expression
   is passed as arg to the macro, and there is an operator
   in the expression with higher preference than the < in the
    macro    This macro evaluates each argument twice. So the i is 
    incremented twice  */ 
#define max(a,b) ((a)>(b)?(a):(b))
biggest = x[0];
i = 1;
while (i < n)     
    biggest = max (biggest, x[i++]); 

#define putc(x,p) (--(p)->_cnt>=0 ? (*(p)->_ptr++=(x)) : _flsbuf(x,p))
putc(c,*f++);  /* might fail, because p argument is evaluated twice */
putc(*c++, f); /* works, because x is evaluated only once. It shows up
                  at both sides of the ternary operator, but only one
                  of them will be executed */
 
#define toupper(c) ((c)>=’a’ && (c)<=’z’? (c)+(’A’-’a’): (c))
toupper(*p++);  /* might fail */
 
#define T1 struct foo *
T1 a, b;  /* expands to struct foo *a, b;
             a is 'struct foo *a' but b is 'struct foo b'
           */
 
typedef struct foo *T2;
T2 a, b;  /* works fine (two pointers to struct foo) */
 
char a = 0xFF;
int b = (int)a;  /* char might be signed or unsigned. if treated as signed
                    the int will be negative. Otherwise it wont
                    To make sure define as 'unsigned char',
                    unsigned char 0 to 255
                    signed char -128 to 127
                  */
 
/* n negative would break things */
int n;
h = n % HASHSIZE;
 
/* fix: */
unsigned n;
/* or */
h = n % HASHSIZE;
if (h < 0)      
    h += HASHSIZE; 

/* breaks it upper passed to toupper */ 
#define toupper(c) ((c)+’A’-’a’)
#define tolower(c) ((c)+’a’-’A’) 
/* evaluates multiple times the argument */
#define toupper(c) ((c) >= ’a’ && (c) <= ’z’? (c) + ’A’ - ’a’: (c))
#define tolower(c) ((c) >= ’A’ && (c) <= ’Z’? (c) + ’a’ - ’A’: (c))
 
 
 
if(a=b) c;         /* a always equals b, but c will be executed if b!=0 */
if( 0 < a < 5) c;  /* 0<a generates 0 or 1, which is always less than 5
                      so this is always false */
if( a =! b) c;    /* assigns !b to a, and evaluates !b for the if */
 
switch (a) {
        int var = 1;    /* This line never runs */
        case A: ...
        case B: ...
        }


static volatile unsigned * const foo = 8; // const pointer. warning int to pointer conversion
char * a = "asdf";  // allocates string in read-only, and a points to it
int * b = 4;        // warning: integer to pointer conversion
int * c = (int *)4;       // correct, but deferencing it will cause sigsev
int * const d = (int *)4; // correct
int * e = (int *)4;       // correct
const int * f = (int *)4; // correct


/*
 * http://pfacka.binaryparadise.com/articles/guide-to-advanced-programming-in-C.html
 */

// typecasting unsigned to signs:

    long i = -1;
    printf("%lu, %lu, %lu, %lu, %lu, %llu, %lld\n",
            sizeof(i),  // same as size(long)
            (size_t)i,  // typecast -1 to unsigned long, which is 0xff ff ff ff ff ff ff ff ff = max 64 bit
            sizeof(size_t),  // 8 bytes, unsigned long
            sizeof(long),
            sizeof(long long), // long long size = long size
            ((long long unsigned)1<<64)-1,  // warning: shift count >= width of type
                                            // doesnt work because 1<<64 overflows and is typecasted to 0
                                            //  so (unsigned)0 - (signed)1 = signed(0-1) = signed int(-1)
                                            //  which is typecasted to llu, which is max 64 bits  
            ((long long unsigned)1<<64)-1); // warning: shift count >= width of type     

// 8, 18446744073709551615, 8, 8, 8, 18446744073709551615, -1



// to avoid double free, or using after freeing
#define nullfree(ptr)	({ assert(!ptr); free(ptr); ptr = 0; })
void nullfree(void **pptr) { 
    void *ptr = *pptr;
    assert(ptr != NULL);      // to catch double free
    free(ptr);
    *pptr = NULL;             // set pointer to null, so it doesnt point to memory freed
}
int main() {
    int *ptr = malloc(10);
    //nullfree(&ptr); // void * is generic, but void ** isnt. this gives warning 
    nullfree((void **)&ptr);

#ifdef EASIER
    nullfree(ptr);
#endif
    return 0;
}


// reference count in C??
// garbage collection in C


// multidimensional array
int p[2][4] = {{1, 2, 3, 4}, {7, 8, 9, 10}};
int element = *(*(p+1)+2); // equivalent to p[1][2]
// to pass it to function, you have to define dimension of inner array
void sum(int data[2][4]) {...}
void sum(int data[][4]) {...}
void sum(int (*data)[4]) {...}
                          

// check for overflow before operation
#include <limits.h>
int a = <something>;
int x = <something>;
if ((x > 0) && (a > INT_MAX - x)) /* `a + x` would overflow */;
if ((x < 0) && (a < INT_MIN - x)) /* `a + x` would underflow */;
if ((x < 0) && (a > INT_MAX + x)) /* `a - x` would overflow */;
if ((x > 0) && (a < INT_MIN + x)) /* `a - x` would underflow */;
if (a > INT_MAX / x) /* `a * x` would overflow */;
if ((a < INT_MIN / x)) /* `a * x` would underflow */;
if ((a == -1) && (x == INT_MIN)) /* `a * x` can overflow */
if ((x == -1) && (a == INT_MIN)) /* `a * x` (or `a / x`) can overflow */


char array_place[100] = "don't panic"; // allocates an array. array_place is a LABEL to the
                                       // memory of the array
char* ptr_place = "don't panic";       // allocates a read-only array, and also a pointer variable
                                       // assigns the address of the array to the pointer
                                       
void foo(char arr[], char *ptr)        // arrays passed to functions are converted to pointers.
                                       // arr becomes a const pointer. Both arguments are the same    
