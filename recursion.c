#include <stdio.h>
#include <stdlib.h>


// If I can go upstairs by 1, 2, or 3 steps at once, count
// the possible ways to go upstairs
int stairs(int steps) {
    if (n<0) 
        return 0;
    else if (n == 0)
        return 1;

    return stairs(steps-1) + stairs(steps-2) + stairs(steps-3)
}

// return all subsets of set
void subsets(int ar[], int n, int m) {
    int i;
    if (n==m) {
        printf("{}, ");
        return;
    } 
    subsets(ar, n+1, m);
    for (i =  n; i < m; i++)
        printf("{%d}, ", ar[i]);
        
