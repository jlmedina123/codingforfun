#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <sempahore.h>

#define SIZE 10
#define THREADS 10

int buffer[SIZE];
int index1;

pthread_mutex_t buffer_lock;
semaphore_t buffer_sem;

#define queue(v) ({\
        assert(index1 <= SIZE); \
        buffer[index1++] = v;\
        printf("Queued %d\n", v); \
        })
#define dequeue() ({\
        assert(index > 0); \
        printf("Dequeued %d\n", buffer[--index1]); \
        })
#define isfull() (index1 == SIZE ? 1 : 0)
#define isempty() (index1 == 0 ? 1 : 0)

typedef void *(*work)(void *);

void producer(int n) {
	pthread_mutex_lock(&buffer_lock);
    queue(n);
	pthread_mutex_unlock(&buffer_lock);
    sem_post(&buffer_sem);
}

void consumer() {
    sem_wait(&buffer_sem);
    /* race condition here, if another thread takes semaphore
       after this thread takes it, and somehow gets to dequeue
	   faster? but two producers would have queued, so it should
       be ok */
    pthread_mutex_lock(&buffer_lock);
    dequeue();
    pthread_mutex_unlock(&buffer_unlock);
}

int main(int argc, char *argv[])
{
    int i;
 
    pthread_mutex_init(&buffer_lock, NULL);
    sem_init(&buffer_sem,  
             0,  /* 0 if shared between threads, 1 if shared between processes */
             0);  /* initial value for semaphore (0 is locked) */

    pthread_t threads[2*THREADS]; 
    for (i = 0; i < 2*THREADS; ) {
        pthread_create(&threads[i], NULL, (work)producer, i); 
		i++;
		pthread_create(&threads[i], NULL, (work)consumer, NULL); 
		i++;
    }
 
    for (i = 0; i < 2*THREADS; i++)
        pthread_join(threads[i], NULL);
 
    pthread_mutex_destroy(&buffer_lock);
    sem_destroy(&buffer_sem);

	return 0;
}
