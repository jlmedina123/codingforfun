/*
https://jlmedina123.wordpress.com/2014/04/10/bucket-sort/

Bucket sort

Below is a simple implementation of bucket sort. It uses half the number of buckets as the numbers of elements in the sequence to store. It goes through all the elements in the sequence, and stores them in buckets. The bucket number to store the element at is given by a hash function. The hash function assumes numbers in sequence are uniformly distributed. Each bucket as a linked list to dynamically store as many elements as it needs.

The runtime of this algorithm would be
– time to distribute elements into buckets: O(n)
– time to retrieve elements from buckets and sort them: O(k) * (O(1) + O(m logm)). where m is the number of elements in bucket. So the average is m = n/k. Since O(n) = O(k), O(m) = O(1).

So the total runtime is O(n) + O(k) * O(1) = O(n + k) = O(n)

*/

#include <stdio.h>
#include <stdlib.h>
 
/* ratio of sequence elements to number of buckets */
#define BUCKETRATIO 2
 
/* for linked list in bucket */
struct node {
    int data;
    struct node *next;
};
typedef struct node node_t;
 
/* each bucket stores elements in a linked list */
struct bucket {
    int size;
    struct node *list;
};
typedef struct bucket bucket_t;
 
void insertsort(int *seq, int left, int right) {
    int i, j, element;
    for (i = left + 1; i < right; i++) {
        element = seq[i];
        for (j = i-1; i >=0 && seq[j] > element; j--) {
            seq[j+1] = seq[j];
        }
        seq[j+1] = element;
    }
}
 
 
int hash(int value, int numb_buckets, int max){
    /* for an uniform distribution, this hash should give the best
       distribution of numbers throughout the buckets */
    int key = value * numb_buckets / max;
    return key;
}
 
void bucketsort(int seq[], int size, int max) {
    int i, j, left;
    int bucket_numb;
    int numb_buckets = size / BUCKETRATIO;
 
    /* allocate and initialize buckets */
    bucket_t *buckets = malloc(sizeof(bucket_t) * numb_buckets);
    for (i = 0; i < numb_buckets; i++) {
        buckets[i].size = 0;
        buckets[i].list = NULL;
    }
    /* distribute array elements into buckets */
    for (i = 0; i < size; i++) {
        bucket_numb = hash(seq[i], numb_buckets, max);
        /* allocate and initialize node to put into bucket */
        node_t *newnode = malloc(sizeof(node_t));
        newnode->data = seq[i];
        newnode->next = NULL;
        /* store node into linked list of bucket bucket_numb */
        buckets[bucket_numb].size++;
        newnode->next = buckets[bucket_numb].list; /* store at first place */
        buckets[bucket_numb].list = newnode;
        printf("Element %d stored in bucket %d (size %d)\n",
               newnode->data, bucket_numb, buckets[bucket_numb].size);
        /* instead, we could store in-order, and then we wouldn't have to
           apply sorting to each list */
    }
    /* go through all buckets. Get elements out and overwrite sequence.
       Then order the section of sequence with elements from bucket */
    j = 0;
    node_t *prev;
    for (i = 0; i < numb_buckets; i++) {
        if (buckets[i].size > 0) {
            left = j;
            node_t *listptr = buckets[i].list;
            while (listptr != NULL) {
                seq[j++] = listptr->data;
                printf("Retrieved %d from bucket %d\n", seq[j-1], i);
                prev = listptr;
                listptr = listptr->next;
                free(prev); /* deallocate memory */
            }
            /* sort subarray */
            insertsort(seq, left, j);
        }
    }
}
 
 
void printseq(int seq[], int size) {
    int i;
    for (i = 0; i < size; i++)
        printf("%d, ", seq[i]);
    printf("\n");
}
 
int main(int argc, int *argv[]) {
    int i;
    int seq[10];
    for (i = 0; i < 10; i++)
        seq[i] = rand() % 100;
 
    printseq(seq, 10);
    bucketsort(seq, 10, 100);
    printseq(seq, 10);
}
