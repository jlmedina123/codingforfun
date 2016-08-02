/*
 * http://www.linuxjournal.com/content/lock-free-multi-producer-multi-consumer-queue-ring-buffer
 *
 */

// wait if full
while (tail_ + Q_SIZE < head_)
        sched_yield();




