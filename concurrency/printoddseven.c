#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
 
sem_t even_sem, odd_sem;
typedef void *(*thread_func)(void *);
 
 
void even() {
    int i = 2;
    while (i < 10) {
        sem_wait(&even_sem);
        printf("%d, ", i);
        i+=2;
        sem_post(&odd_sem);
    }
}
 
void odd() {
    int i = 1;
    while (i < 10) {
        sem_wait(&odd_sem);
        printf("%d, ", i);
        i+=2;
        sem_post(&even_sem);
    }
}
 
int main() {
    sem_init(&even_sem, 0, 0);
    sem_init(&odd_sem, 0, 1);
    pthread_t threadodd, threadeven;
    pthread_create(&threadodd, NULL, (thread_func)odd, NULL);
    pthread_create(&threadeven, NULL, (thread_func)even, NULL);
    pthread_join(threadodd, NULL);
    pthread_join(threadeven, NULL);
    sem_destroy(&even_sem);
    sem_destroy(&odd_sem);
    return 0;
}
