

volatile unsigned long last_head_;
volatile unsigned long last_tail_;

volatile unsigned long head, tail;

struct ThrPos {
    volatile unsigned long head, tail;
};

ThrPos thr_p_[std::max(n_consumers_, n_producers_)];


void push(T *ptr)
{
    thr_pos().head = head_;
    thr_pos().head = __sync_fetch_and_add(&head_, 1);

    while (__builtin_expect(thr_pos().head >=
                            last_tail_ + Q_SIZE, 0))
    {
        ::sched_yield();

        auto min = tail_;
        for (size_t i = 0; i < n_consumers_; ++i) {
            auto tmp_t = thr_p_[i].tail;

            asm volatile("" ::: "memory"); // compiler barrier

            if (tmp_t < min)
                min = tmp_t;
        }
        last_tail_ = min;
    }

    ptr_array_[thr_pos().head & Q_MASK] = ptr;
    thr_pos().head = ULONG_MAX;
}

T *pop()
{
    thr_pos().tail = tail_;
    thr_pos().tail = __sync_fetch_and_add(&tail_, 1);

    while (__builtin_expect(thr_pos().tail >=
                            last_head_, 0))
    {
        ::sched_yield();

        auto min = head_;
        for (size_t i = 0; i < n_producers_; ++i) {
            auto tmp_h = thr_p_[i].head;
       
            asm volatile("" ::: "memory"); // compiler barrier

            if (tmp_h < min)
                min = tmp_h;
        }
        last_head_ = min;
    }

    T *ret = ptr_array_[thr_pos().tail & Q_MASK];
    thr_pos().tail = ULONG_MAX;
    return ret;
}
