#include <stdio.h>
 
char * func1(char *a) {
    *a = 'b';
    return a;
}
 
char * func2(char *a) {
    *a = 'c';
    return a;
}
 
int main() {
    char a = 'a';
    /* declare array of function pointers
     * the function pointer types are char * name(char *)
     * A pointer to this type of function would be just
     * put * before name, and parenthesis around *name:
     *   char * (*name)(char *)
     * An array of these pointers is the same with [x]
     */
    char * (*functions[2])(char *) = {func1, func2};
    printf("%c, ", a);
    printf("%c, ", *(functions[0](&a)));   
    printf("%c\n", *(*functions[1])(&a)); // seems to work both ways
 
    a = 'a';
    /* creating 'name' for a function pointer type
     * funcp is equivalent to type char *(*funcname)(char *)
     */
    typedef char *(*funcp)(char *);
    /* Now the declaration of the array of function pointers
     * becomes easier
     */
    funcp functions2[2] = {func1, func2};
 
    printf("%c, ", a);
    printf("%c, ", *(functions2[0](&a)));
    printf("%c\n", *(functions2[1](&a)));
 
    functions2[0](&a);
    printf("%c\n", a);
    return 0;
}
