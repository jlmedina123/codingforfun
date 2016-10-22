#include <stdio.h>
#include <stdlib.h>
 
void diamond_easier(int lines) {
    int i, j, spaces, starts;
    if (lines %2 == 0) lines++;
 
    spaces = (lines-1)/2;
    starts = 1;
    for (i = 1; i <= (lines+1)/2; i++) {
        for (j=spaces; j; j--)
            printf(" ");
        for (j=starts; j; j--)
            printf("*");
        printf("\n");
        spaces--;
        starts+=2;
     }
     spaces = 1;
     starts = lines-2;
     for (i = 1; i <= (lines-1)/2; i++) {
         for (j = spaces; j; j--)
             printf(" ");
         for (j = starts; j; j--)
             printf("*");
         printf("\n");
         spaces++;
         starts-=2;
     }
}
 
void diamond1(int lines) {
    // make sure number lines is odd
    if (lines%2 == 0) lines++;
 
    int l, i;
    /* upper part (and central line)
       space_left + starts + space_right = lines
       space_left = space_right = space
       2*space + starts = lines
       starts = 2*line_no -1
       space = (lines - 2*line_no +1)/2
             = (lines+1)/2 - 2*line_no
    */
    for (l = 1; l<=(lines+1)/2; l++) {
        for (i = 1; i<=(lines-2*l+1)/2; i++)
            printf(" ", i);
        // starts
        for (i = 1; i<=2*l-1; i++)
            printf("*");
        printf("\n");
    }
    /* bottom part
       2*space + starts = lines
       starts = 2*(lines - line_no) + 1
       space = (lines - 2*lines + 2*line_no - 1)/2
             = (2*line_no - lines -1)/2
             = line_no - (lines+1)/2
    */
    for( ; l<=lines; l++) {
        for (i = 1; i <= (2*l-lines-1)/2; i++)
            printf(" ", i);
        // starts
        for (i = 1 ; i<= 2*(lines-l) + 1; i++)
            printf("*");
        printf("\n");
    }
}
 
int diamond2(int lines) {
    if (lines%2==0) lines++;
    int l, c;
    int stars = 1;
    int spaces = (lines - stars)/2; // 2*spaces+stars=lines
    for (l = 1; l <= (lines+1)/2; l++) {
        for (c = 1; c <= spaces; c++)
            printf(" ");
        for (c = 1; c <=stars; c++)
            printf("*");
        stars+=2;
        spaces--;
        printf("\n");
    }
    spaces = 1;
    stars = (lines+1)/2+1;
    for ( ;l <= lines; l++) {
        for (c = 1; c <= spaces; c++)
            printf(" ");
        for (c = 1; c <=stars; c++)
            printf("*");
        stars-=2;
        spaces++;
        printf("\n");
    }
}
 
 
int main() {
    diamond1(7);
    diamond2(7);
 
    return 0;
}
