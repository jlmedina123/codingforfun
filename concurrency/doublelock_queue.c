#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

struct node {
    struct node *next;
    int value
};

struct queue {
    struct node *head;  // dequeue from head
    struct node *tail;  // enqueue in tail
    mutex_t head_lock;
    mutex_t tail_lock;
};

void enqueue(struct queue *q, int value) {
    struct node * new = malloc(sizeof(struct node));
    new->value = value;
    assert(new && q);
    new->next = NULL;

    // enqueue at tail. Always at least
    // one dummy node (even if queue empty)
    mutex_lock(&q->tail_lock);
    q->tail->next = new;
    q->tail = new;
    mutex_unlock(&q->tail_lock);    

#ifdef NODUMMY
    // without dummy node, we would have to acquire
    // both locks
    mutex_lock(&q->tail_lock);
    if (tail) {
        q->tail->next = new;
        q->tail = new;
    } else {
        q->tail = new;
        // WRONG: possible deadlock
        mutex_lock(&q->head_lock);
        q->head = new;
        mutex_unlock(&q->head_lock);
    }
    mutex_unlock(&q->tail_lock);
#endif
}

int dequeue(struct queue *q) {
    assert(q);

    mutex_lock(&q->head_lock);
    if (q->head->next == NULL) {
        // empty list
        mutex_unlock(&q->head_lock);
        return 0;
    }
    int val = q->head->value;
    mutex_unlock(&q->head_lock);
    return val;
}


int main() {


    struct queue q;
    init_mutex(&a.tail_head);
    init_mutex(&a.tail_lock);
    struct node dummy;
    node.next = NULL;
    node.value = 0;
    q.head = &dummy;
    q.tail = &dummy;
    
    pthread_t threads[10];
    int values[] = {4, 5, 2, 8, 9};
    int i;
    for (i = 0; i < 10; )
        pthread_create(&threads[i], NULL, (funcp)producer, NULL);
        i++;
        pthread_create(&threads[i], NULL, (funcp)consumer, NULL); 
        i++;
    } 
    // LEFT OFF


