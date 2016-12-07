#include <stdio.h>
#include <stdlib.h>

void swap(char *a, char *b) { 
	char t = *a; 
	*a = *b; 
	*b = t; 
}


void permutations_bell() {
	// TBD
}

void permutations_backtracking(char *a, int i, int n) {
   int j; 
   // if last letter, print string
   if (i == n -1)
       printf("%s\n", a);
   else {
	   // for each letter	
       for (j = i; j < n; j++) {
          swap((a+i), (a+j));
		  // get
          permute(a, i+1, n);
		  // backtrack and undo original swap
          swap((a+i), (a+j)); 
       }
   }
} 


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

	//...
}


int main() {
    /* recursive binary search of ordered array */

    /* print all permutations of a string */

    /* print all combinations of a string */


    return 0;
}
