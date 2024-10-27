#pragma once

#include <mixins/concurrent/atomic.h>

using lock_t = Atomic<int>;

static inline bool
CheckInterrupts()
{
#if defined(__i386__) || defined(__x86_64__)
    volatile u64 flags;
    asm volatile("pushfq;"
                 "pop %0;"
                 : "=rm"(flags)::"memory", "cc");
    return flags & 0x200;
#elif defined(__arm__)
    return false;
#endif
}

#define acquireLock(lock)                                                      \
    ({                                                                         \
        while (lock.Exchange(1, MemoryOrderAcquired))                          \
            asm("pause");                                                      \
    })

#define acquireLockIntDisable(lock)                                            \
    ({                                                                         \
        asm volatile("cli");                                                   \
        while (lock.Exchange(1, MemoryOrderAcquired))                          \
            asm volatile("sti; pause; cli");                                   \
    })

#define releaseLock(lock) ({ lock.Store(0, MemoryOrderRelease); });

#define releaseLockIntEnable(lock)                                             \
    ({                                                                         \
        lock.Store(0, MemoryOrderRelease);                                     \
        asm volatile("sti");                                                   \
    })

class Spinlock
{
public:
    inline void Acquire() { acquireLock(m_lock); }

    inline void Release() { releaseLock(m_lock); }

private:
    lock_t m_lock{ 0 };
};

template <bool DisableInterrupts = false>
class ScopedLock final
{
public:
    __attribute__((always_inline)) inline ScopedLock(lock_t& lock)
        : m_lock(lock)
    {
        if constexpr (DisableInterrupts) {
            m_irq = CheckInterrupts();
            if (m_irq) {
                acquireLockIntDisable(m_lock);
            } else {
                acquireLock(m_lock);
            }
        } else {
            acquireLock(m_lock);
        }
    }

    __attribute__((always_inline)) inline ~ScopedLock()
    {
        releaseLock(m_lock);

        if constexpr (DisableInterrupts) {
            if (m_irq) {
                asm volatile("sti");
            }
        }
    }

private:
    lock_t& m_lock;
    bool    m_irq{ false };
};