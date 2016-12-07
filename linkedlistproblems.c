#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
 
#define N 19 // hash table size
#define EMPTY -1
#define TRUE 1
#define FALSE 0
 
struct node {
    struct node * next;
    int value;
};
typedef struct node node_t;
 
 
int haskey(int table[N][5], int value) {
    int key = value % N;
    int i = 0;
    while (table[key][i] != -1) {
        if (table[key][i] == value)
            return TRUE;
        i++;
    }
    return FALSE;
}
 
void insertvalue(int table[][5], int value) {
    int key = value % N;
    int i = 0;
    // checking if collision
    while (table[key][i] != -1) {
        i++;
        if (i == 5) // too many collisions
            return; // discard value
    }
    // insert value at key position
    table[key][i] = value;
    if (i < 4)
        table[key][++i] = -1;
}
 
/*
 * remove duplicates in unsorted list
 */
void remove_duplicates(node_t * list) {
    /* solution 1: sweep in two nested loos (O(N2))
       solution 2: put elements in buffer (O(N)) and
       sort elements (O(N*logN)) and sweep list (O(N)),
       checking if element in buffer (O(logN). Total O(NlogN)
       solution 3: use hash or bit array. O(N)
    */
    if (list == NULL || list->next == NULL)
        return;
 
    /* create hash table */
    int i;
    int hashtable[N][5]; // using array for collisions (max 5 key collisions)
    // for (i = 0; i < N; i++)
    //    hashtable[i][0] = EMPTY; // -1 indicates no value for the key
    memset(hashtable, EMPTY, sizeof(hashtable));
 
    /* sweep list */
    insertvalue(hashtable, list->value); // first element
 
    node_t * current = list->next;
    node_t * prev = list;
    node_t * duplicate;
    while (current != NULL) {
        if (haskey(hashtable, current->value)) {
            duplicate = current;
            /* skip node */
            prev->next = current->next;
            /* move current, prev doesnt move */
            current = current->next;
            /* free duplicate node */
            free(duplicate);
        } else {
            insertvalue(hashtable, current->value);
            /* move forward */
            prev = current;
            current = current->next;
        }
    }
}
 
void insert_beginning(node_t ** list, int value) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->value = value;
    // newnode->next = NULL;
    newnode->next = *list;
    *list = newnode;
}
 
void print_list(node_t * list) {
    while (list != NULL) {
        printf("%d, ", list->value);
        list = list->next;
    }
    printf("\n");
}
 
/*
 * Remove node of list having access to only that node
 */
void remove_node(node_t *node) {
    node_t *next = node->next;
    memcpy(node, node->next, sizeof(node_t));
    free(next);
}

/*
 * return node k nodes from the last node 
 */
node_t * nodefromend(node_t *list, int k) {
    // move pointer to node k-th. Then
    // start another pointer from head, 
    // and move prev and new pointer together
    // until end
    node_t *first = list;
    node_t *second = list;
    int i = 0;
    while (first != NULL && i < k) {
        first = first->next;
        i++;
    }
    if (first == NULL)
        return NULL;
    while (first != NULL) {
        first = first->next;
        second = second->next;
    }
    return second;
}
 
 
/*
 * detect if list has loop
 */
int detectloop(node_t *list) {
    /* using slow ptr and fast ptr that
       advances two nodes at a time. If there is
       loop, they both will get into loop, and eventually
       fast ptr will point reach slow ptr and point to
       same node
     */
    if (list == NULL)
        return FALSE;
 
 
    node_t *slow = list;
    node_t *fast = list;
    while (fast && fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
        if (fast == slow)
            return TRUE;
    }
    return FALSE;
}
 
int ispalindrome(node_t * list) {
 
    if (list == NULL || list->next == NULL)
        return FALSE;
 
    int queue[40];
    int index = 0;
    node_t * fast = list;
    node_t * slow = list;
    /* traverse list up to middle and put elements
       in a queue. use fast/slow method to stop at
       middle */
    while (fast != NULL && fast->next != NULL &&\
           fast->next->next != NULL) {
        queue[index++] = slow->value;
        slow = slow->next;
        fast = fast->next->next;
    }
    /* if list odd lenght, skip element in middle */
    if (fast->next != NULL)
        slow = slow->next;
 
    /* move slow to end, and compare with LIFO queue */
    while (slow != NULL) {
        if (slow->value != queue[--index])
            return FALSE;
        slow = slow->next;
    }
    return TRUE;
}
 
void readbackwards(node_t * list) {
    if (!list)
        return;
    readbackwards(list->next);    
    printf("%d, ", list->value); 
}

int main() {
 
    node_t * list = NULL;
    int a[8] = {4, 2, 7, 4, 3, 7, 9, 2};
    int i;
    for (i = 0; i < 8; i++) {
        insert_beginning(&list, a[i]);
        print_list(list);
    }
    /* remove duplicate nodes in unsorted list */
    remove_duplicates(list);
    print_list(list);
 
    /* return node k nodes from the end */
    node_t * node = nodefromend(list, 3);
    assert(node!=NULL);
    printf("%d\n", node->value);
 
    /* remove a node, give ptr to that node only */
    remove_node(node);
    print_list(list);
 
    /* detect if there is loop (if list is cyclical) */
    if (detectloop(list))
        printf("Loop detected\n");
    else
        printf("Loop not detected\n");
 
    /* check if list is palindrome */
    if (ispalindrome(list))
        printf("List is palindrome\n");
    else
        printf("List is not palindrome\n");

    /* read list backwards without using additional storage */
    printf("List read backwards:\n");
    readbackwards(list);
    printf("\n");
    return 0;
}
