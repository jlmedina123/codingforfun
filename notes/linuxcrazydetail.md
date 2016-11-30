
https://0xax.gitbooks.io/linux-insides/content/Interrupts/interrupts-1.html

http://web.eecs.utk.edu/~huangj/cs361/students/spinlock_tsurkan.html


### Interrupts:

* Old machines PIC
* APIC: two parts
    * Local APIC: one on each CPU core. Responsible for handling the CPU-specific interrupt configuration. Interrupts from local CPU devices (APIC timer, thermal sensors, etc)
    * I/O APIC: provides multiprocessor interrupt management. Distributes external interrupts among CPU cores

* Interrupt Descriptor Table (IDT): maintains address of each interrupt handler. 
* Processor uses vector number as index to IDT (0 - 255). 0-31 reserved for archictecture defined interrupts and exceptions. 32-255 for user-defined interrupts
* Types of interrupts:
    * external or hardware generated: async
    * software generated or exceptions: synchronous. 3 types
        * faults: reported before the execution of a faulty instruction. fault is corrected and program continues
        * traps: reported after execution of the trap instruction
        * aborts: does not report the exact instruction that caused it. program can't continue


# Synchronization primitives

## spinlock

* spinlock: if `CONFIG_DEBUG_LOCK_ALLOC` disabled:

```
typedef struct spinlock {
        union {
              struct raw_spinlock rlock;
        };
} spinlock_t;
```

* raw spinlock:

```
typedef struct raw_spinlock {
        arch_spinlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
        unsigned int break_lock;
#endif
} raw_spinlock_t;
```

break_lock (if `CONFIG_GENERIC_LOCKBREAK` enabled): set to 1 if CPU waiting on lock. To prevent long time locking

* arch spinlock (for x84_64):

```
#ifdef CONFIG_QUEUED_SPINLOCKS
#include <asm-generic/qspinlock_types.h>
#else
typedef struct arch_spinlock {
        union {
                __ticketpair_t head_tail;
                struct __raw_tickets {
                        __ticket_t head, tail;
                } tickets;
        };
} arch_spinlock_t;
```

If CONFIG_QUEUED_SPINLOCKS enabled:

```
typedef struct qspinlock {
    atomic_t    val;
} arch_spinlock_t;
```


### spinlock_init

```
#define __RAW_SPIN_LOCK_UNLOCKED(lockname)  (raw_spinlock_t)__RAW_SPIN_LOCK_INITIALIZER(lockname)
#define __RAW_SPIN_LOCK_INITIALIZER(lockname)  *(&(_lock)->rlock) = __ARCH_SPIN_LOCK_UNLOCKED
#define __ARCH_SPIN_LOCK_UNLOCKED       { ATOMIC_INIT(0) }
```

### spin_lock()

static __always_inline void spin_lock(spinlock_t *lock) -> raw_spin_lock(&lock->rlock)


```
static inline void __raw_spin_lock(raw_spinlock_t *lock)
{       
        preempt_disable(); // other processes must be prevented to preempt the process which acquired a lock
        spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
        LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}

#define spin_acquire(l, s, t, i)    lock_acquire_exclusive(l, s, t, NULL, i)
#define lock_acquire_exclusive(l, s, t, n, i)  lock_acquire(l, s, t, 0, 1, n, i)

void lock_acquire(...) {

         raw_local_irq_save(flags); // disable hardware interrupts (process wont be preempted)
         check_flags(flags);

         current->lockdep_recursion = 1;
         trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);
         __lock_acquire(lock, subclass, trylock, read, check,
                        irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
         current->lockdep_recursion = 0;
         raw_local_irq_restore(flags);
}

#define LOCK_CONTENDED(_lock, try, lock) lock(_lock)

```



