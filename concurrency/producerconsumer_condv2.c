#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define SIZE 10
#define THREADS 10
#define LOOPS 10

int buffer[SIZE];
int index1;

pthread_mutex_t buffer_lock;
pthread_cond_t full_cond;
pthread_cond_t empty_cond;

#define queue(v) ({\
        assert(index1 <= SIZE); \
        buffer[index1++] = v;\
        })
#define dequeue() (buffer[--index1])
#define isfull() (index1 == SIZE ? 1 : 0)
#define isempty() (index1 == 0 ? 1 : 0)

typedef void *(*work)(void *);

void producer(int t_num) {
    int i = 0;
    while (i++ < LOOPS) {
        sleep(random() % 10);
        pthread_mutex_lock(&buffer_lock);
        while (isfull())
            pthread_cond_wait(&full_cond, &buffer_lock);
        printf("Queueing %d\n", i);
        queue(i);
        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&empty_cond);    
    }
}

void consumer(int t_num) {
    int i = 0;
    int v;
    while (i++ < LOOPS) {
        sleep(random() % 10);
        pthread_mutex_lock(&buffer_lock);
        while (isempty())
            pthread_cond_wait(&empty_cond, &buffer_lock);
        v = dequeue();
        printf("Dequeing %d\n", v);
        pthread_mutex_unlock(&buffer_lock);    
        pthread_cond_signal(&full_cond);
    }
}

int main(int argc, char *argv[]) {
    int buffer_index = 0;
    pthread_t thread[THREADS];
    int thread_num[THREADS];
    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&empty_cond, NULL);
    pthread_cond_init(&full_cond, NULL);

    int i = 0;
    for (i = 0; i < THREADS; ) {
        thread_num[i] = i;
        pthread_create(&thread[i],   // pthread_t *thread
                       NULL,         // const pthread_attr_t *attr
                       (work)producer,     // void *(*start_routine) (void *)
                       &thread_num[i]);          // void *arg
        i++;

        thread_num[i] = i;
        pthread_create(&thread[i],
                       NULL,
                       (work)consumer,
                       &thread_num[i]);
        i++;
    }
    for (i = 0; i < THREADS; i++)
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&buffer_lock);
    pthread_cond_destroy(&full_cond);
    pthread_cond_destroy(&empty_cond);

    return 0;
}
