#pragma once

#define acquireLock(lock)                                                      \
    ({                                                                         \
        while (__atomic_exchange_n(lock, 1, __ATOMIC_ACQUIRE))                 \
            asm("pause");                                                      \
    })

#define acquireLockIntDisable(lock)                                            \
    ({                                                                         \
        asm volatile("cli");                                                   \
        while (__atomic_exchange_n(lock, 1, __ATOMIC_ACQUIRE))                 \
            asm volatile("sti; pause; cli");                                   \
    })

#define releaseLock(lock) ({ __atomic_store_n(lock, 0, __ATOMIC_RELEASE); });

#define releaseLockIntEnable(lock)                                             \
    ({                                                                         \
        __atomic_store_n(lock, 0, __ATOMIC_RELEASE);                           \
        asm volatile("sti");                                                   \
    })

class Spinlock
{
public:
    inline void AcquireIntDisable() { acquireLockIntDisable(&m_lock); }

    inline void Acquire() { acquireLock(&m_lock); }

    inline void Release() { releaseLock(&m_lock); }

private:
    volatile int m_lock{ 0 };
};