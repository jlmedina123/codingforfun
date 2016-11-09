/* My own implementation for nonblocking concurrent linked list.
   It doesnt solve the ABA problem. See Harri's solution for 
   more complete algorithm: 

   https://en.wikipedia.org/wiki/Non-blocking_linked_list 

   Using CAS synchronization from gcc atomic builtins
   http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html

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

   The “bool” version returns true if the comparison is 
   successful and newval was written.
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


struct node {
     struct node * next;
     int value;
};

struct node * list = NULL;

/* Using Compare And Swap
    1. Set new->next to list first node (*list)
    2. Atomically check if first node has changed. If it hasnt, set new as first node
       If it has, try again. Set new->next to the new first node...
*/
void insertorder(struct node ** list, struct node *new) {
     int value = new->value;
     int ret = 0;
     printf("inserting %d, ", value);

     /* if inserting at first node */
    if (!*list || (*list)->value > value) {
        ret = 1;
        do {
             /* Another thread might have beat us and added
              * a smaller node in front
              */
             if (*list && (*list)->value < value) {
                 ret = 0;
                 break;
             }
             new->next = *list;
         } while(!__sync_bool_compare_and_swap(
                     list,// ptr to update (first node), type struc node ** 
                     new->next, // old value (I copied it into here), type struct node * 
                     new));     // list should now point to new node, type struct node *
     }
     if (ret) return;

     /* if inserting after first node */
     struct node *ptr;
     struct node *prev;
     do {
         ptr = (*list)->next;
         prev = *list;
         // traverse list
         while (ptr && ptr->value < value) {
             prev = ptr;
             ptr = ptr->next;
         }
         // try to insert
         new->next = ptr;
         //prev->next = new;  // CAS:      
     } while (!__sync_bool_compare_and_swap(
                 &(prev->next), // ptr to update
                 ptr,  // old value (points to currently next node) 
                 new));  // we want it to point to our new node
}

void insertfirst(struct node ** list, struct node *new) {
    printf("inserting %d, ", new->value);
    do {
        new->next = *list;
    } while (!__sync_bool_compare_and_swap(list,       // pointer to address of first node 
                                           new->next,  // old address 
                                           new));      // new address
}

void readlist(struct node *list) {
    printf("\n");
    while (list) {
        printf("%d, ", list->value);
        list = list->next;
    }
    printf("\n");
}

typedef void (*insert_fp)(struct node **, struct node *);
typedef void *(*thread_fp)(void *);

void producer(insert_fp insert) {
    int i = 0;
    struct  node *new;
    while (i++ < 5) {
        new = calloc(sizeof(struct node), 1);
        new->value = rand() % 1000;
        insert(&list, new);
    }
}

int main() {
    pthread_t threads[10];
    int i;

    for (i = 0; i < 10; i++)
        pthread_create(&threads[i], NULL, (thread_fp)producer, insertfirst);
    for (i = 0; i < 10; i++)
        pthread_join(threads[i], NULL);
    readlist(list);

    list = NULL;
    for (i = 0; i < 10; i++)
        pthread_create(&threads[i], NULL, (thread_fp)producer, insertorder);
    for (i = 0; i < 10; i++)
        pthread_join(threads[i], NULL);
    readlist(list);

    return 0;
}

