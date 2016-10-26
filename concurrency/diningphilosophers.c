/* https://en.wikipedia.org/wiki/Dining_philosophers_problem */

// Layout of the table (P = philosopher, f = fork):
//          P0
//       f3    f0
//     P3        P1
//       f2    f1
//          P2

mutex_t forks[4];

void eatandthink(int philosopher_num) {


int main() {
    for (i = 0; i < 4; i++) 
        pthread_mutex_init(forks[i]);

    pthread_t philosophers[4];
