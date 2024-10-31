#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>

namespace concurrent {

}

enum MemoryOrder : int
{
    MemoryOrderRelaxed  = __ATOMIC_RELAXED,
    MemoryOrderConsume  = __ATOMIC_CONSUME,
    MemoryOrderAcquired = __ATOMIC_ACQUIRE,
    MemoryOrderRelease  = __ATOMIC_RELEASE,
    MemoryOrderAcqRel   = __ATOMIC_ACQ_REL,
    MemoryOrderSeqCst   = __ATOMIC_SEQ_CST,
};
MakeFlags$(MemoryOrder);

using AtomicFlags = Flags<MemoryOrder>;

constexpr static inline u32 MemoryOrderMask         = 0x0ffff;
constexpr static inline u32 MemoryOrderModifierMask = 0xffff0000;
constexpr static inline u32 MemoryOrderHleAcquire   = 0x10000;
constexpr static inline u32 MemoryOrderHleRelease   = 0x20000;

template <typename TType>
class Atomic
{
public:
    constexpr Atomic()
        requires Constructible<TType>
        : m_value()
    {
    }

    constexpr Atomic(TType const& val)
        : m_value(val)
    {
    }

    constexpr Atomic(TType&& val)
        : m_value(val)
    {
    }

    always_inline TType Exchange(TType       desired,
                                 MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_exchange_n(&m_value, desired, (int)order);
    }

    always_inline bool CompareAndExchange(TType       expected,
                                          TType       desired,
                                          MemoryOrder order = MemoryOrderSeqCst)
    {
        if (order == MemoryOrderAcqRel || order == MemoryOrderRelaxed)
            return __atomic_compare_exchange_n(&m_value,
                                               &expected,
                                               desired,
                                               false,
                                               MemoryOrderRelaxed,
                                               MemoryOrderAcquired);

        return __atomic_compare_exchange_n(
            &m_value, &expected, desired, false, order, order);
    }

    always_inline TType FetchAdd(TType       val,
                                 MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_add(&m_value, val, order);
    }

    always_inline TType FetchSub(TType       val,
                                 MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_sub(&m_value, val, order);
    }

    always_inline TType FetchAnd(TType       val,
                                 MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_and(&m_value, val, order);
    }

    always_inline TType FetchOr(TType       val,
                                MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_or(&m_value, val, order);
    }

    always_inline TType FetchXor(TType       val,
                                 MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_xor(&m_value, val, order);
    }

    always_inline TType FetchInc(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_add(&m_value, 1, order);
    }

    always_inline TType FetchDec(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_sub(&m_value, 1, order);
    }

    always_inline void Inc(MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_add_fetch(&m_value, 1, order);
    }

    always_inline void Dec(MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_sub_fetch(&m_value, 1, order);
    }

    always_inline TType Load(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_load_n(&m_value, order);
    }

    always_inline void Store(TType val, MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_store_n(&m_value, val, order);
    }

    always_inline bool IsLockFree()
    {
        return __atomic_is_lock_free(sizeof(TType), &m_value);
    }

private:
    TType m_value;
};