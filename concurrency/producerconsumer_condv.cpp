<<<<<<< HEAD



unsigned long head_;
unsigned long tail_;

T *ptr_array_[Q_SIZE];

ptr_array_[tail_++ & Q_MASK]


// To get the current position in the array, we can calculate a remainder of integer division of tail_ by Q_SIZE, but rather we define Q_SIZE as a power of 2 (32768 in our case), so we can use bitwise AND between Q_MASK and tail_, which is bit faster
static const unsigned long Q_MASK = Q_SIZE - 1;

std::condition_variable cond_empty_;
std::condition_variable cond_overflow_;
std::mutex mtx_;

void push(T *x)
{
    std::unique_lock<std::mutex> lock(mtx_);

    cond_overflow_.wait(lock, [&head_, &tail_]() {
                    return tail_ + Q_SIZE > head_;
            });

    ptr_array_[head_++ & Q_MASK] = x;

    cond_empty_.notify_one();
}

T *pop()
{
    std::unique_lock<std::mutex> lock(mtx_);

    cond_empty_.wait(lock, [&head_, &tail_]() { return tail_ < head_; });

    T *x = ptr_array_[tail_++ & Q_MASK];

    cond_overflow_.notify_one();

    return x;
}
