
/* Non-blocking concurrent linked list based on Harri's solution:
   https://en.wikipedia.org/wiki/Non-blocking_linked_list 
 
 gcc atomic builtins (http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html)

type __sync_fetch_and_add (type *ptr, type value, ...)
type __sync_fetch_and_sub (type *ptr, type value, ...)
type __sync_fetch_and_or (type *ptr, type value, ...)
type __sync_fetch_and_and (type *ptr, type value, ...)
type __sync_fetch_and_xor (type *ptr, type value, ...)
type __sync_fetch_and_nand (type *ptr, type value, ...)

type __sync_add_and_fetch (type *ptr, type value, ...)
type __sync_sub_and_fetch (type *ptr, type value, ...)
type __sync_or_and_fetch (type *ptr, type value, ...)
type __sync_and_and_fetch (type *ptr, type value, ...)
type __sync_xor_and_fetch (type *ptr, type value, ...)
type __sync_nand_and_fetch (type *ptr, type value, ...)

bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)

The “bool” version returns true if the comparison is successful and newval was written.

*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define METHOD1

struct node {
     struct node * next;
     int value;
};

struct node * list = NULL;
#ifdef METHOD1
int 
#else
void
#endif
insertorder(struct node ** list, struct node *new) {
    /* Search for the right spot in the list
       Insert using CAS
     */
     int value = new->value;
     printf("Inserting %d\n", value);
     /* if list empty or first element greater than new element */      
     if (!*list || (*list)->value <= value) {
         new->next = *list;
         /* swapping atomically *list to point to new if *list hasnt changed */
         /* it *list changes by the time we get here, another thread inserted
            from the if instruction above to here. In that case we might be
            inserting in the wrong order
          */
          /* Using CAS: swapping *list only if it hasnt changed since
             previous instruction */
          if (__sync_bool_compare_and_swap(list,      // ptr to update (first node), type struc node ** 
                                           new->next, // old value (I copied it into here), type struct node * 
                                           new))      // list should now point to new node, type struct node *
#ifdef METHOD1
              return 0; // success
#else
              return;
#endif              

          else
              /* another thread beat us from the 'if(!*list ...)' instruction to here
                 We can do two things: we can just fail, or try the whole thing again
               */
#ifdef METHOD1              
              return -1;
#else
              /* the list has changed since we checked last time, 
               * Try from scratch again 
               */
              return insertorder(list, new); 
#endif              
                     
     }

     /* sweep list to find where to insert */
     struct node *ptr = *list;
     struct node *prev = NULL;
     while (ptr && ptr->value <= value) {
         prev = ptr;
         ptr = ptr->next;
     }
     new->next = ptr;                 
     if (__sync_bool_compare_and_swap(&(prev->next), // ptr to update
                                      ptr,  // old value (points to currently next node) 
                                      new))  // we want it to point to our new node
#ifdef METHOD1
         return 0;
#else
         return;
#endif
     else
         /* another thread beat us */
#ifdef METHOD1
         return -1;
#else
         return insertorder(list, new);
#endif  
}

void insertfirst(struct node ** list, struct node *new) {

    /* Using Compare And Swap
       1. Set new->next to list first node (*list)
       2. Atomically check if first node has changed. If it hasnt, set new as first node
          If it has, try again. Set new->next to the new first node...
     */
    do {
        new->next = *list;
    } while (!__sync_bool_compare_and_swap(list,       // pointer to address of first node 
                                           new->next,  // old address 
                                           new));      // new address
} 

void readlist(struct node *list) {
    while (list) {
        printf("%d, ", list->value);
        list = list->next;
    }
    printf("\n");
}

typedef void *(*thread_fp)(void *);

void producerfirst(int *arg) {
    int i = 0;
    struct  node *new;
    while (i++ < 5) {
        new = calloc(sizeof(struct node), 1);
        new->value = *arg;  
        insertfirst(&list, new);
    }      
}

void producerorder(int *arg) {
    int i = 0;
    struct  node *new;
    while (i++ < 5) {
        new = calloc(sizeof(struct node), 1);
        new->value = *arg;
        if(!insertorder(&list, new)) printf("Failed to insert %d\n", *arg);
    }
}

int main() {
    pthread_t threads[10];
    int values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
/*
    for (i = 0; i < 10; i++)
        pthread_create(&threads[i], NULL, (thread_fp)producerfirst, &values[i]);

    for (i = 0; i < 10; i++)
        pthread_join(threads[i], NULL);

    readlist(list);
*/
    //list = NULL;
    for (i = 0; i < 10; i++)
        pthread_create(&threads[i], NULL, (thread_fp)producerorder, &values[i]);

    for (i = 0; i < 1; i++)
        pthread_join(threads[i], NULL);

    readlist(list);   
}
