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

template <typename _T>
class Atomic
{
public:
    Atomic(_T const& val)
        : m_value(val)
    {
    }

    Atomic(_T&& val)
        : m_value(val)
    {
    }

    _T exchange(_T desired, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_exchange_n(&m_value, desired, (int)order);
    }

    bool compareAndExchange(_T          expected,
                            _T          desired,
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

    _T fetchAdd(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_add(&m_value, val, order);
    }

    _T fetchSub(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_sub(&m_value, val, order);
    }

    _T fetchAnd(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_and(&m_value, val, order);
    }

    _T fetchOr(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_or(&m_value, val, order);
    }

    _T fetchXor(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_xor(&m_value, val, order);
    }

    _T fetchInc(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_add(&m_value, 1, order);
    }

    _T fetchDec(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_fetch_sub(&m_value, 1, order);
    }

    void inc(MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_add_fetch(&m_value, 1, order);
    }

    void dec(MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_sub_fetch(&m_value, 1, order);
    }

    _T load(MemoryOrder order = MemoryOrderSeqCst)
    {
        return __atomic_load_n(&m_value, order);
    }

    void store(_T val, MemoryOrder order = MemoryOrderSeqCst)
    {
        __atomic_store_n(&m_value, val, order);
    }

    bool isLockFree() { return __atomic_is_lock_free(sizeof(_T), &m_value); }

private:
    _T m_value;
};