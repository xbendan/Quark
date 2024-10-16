#pragma once

#include <mixins/std/concepts.h>
#include <mixins/std/type_traits.h>

#define MakeFlags$(T)                                                          \
    inline T operator~(T rhs)                                                  \
    {                                                                          \
        using U = Std::UnderlyingType<T>;                                      \
        return (T) ~(U)rhs;                                                    \
    }                                                                          \
    inline T operator|(T lhs, T rhs)                                           \
    {                                                                          \
        using U = Std::UnderlyingType<T>;                                      \
        return (T)((U)lhs | (U)rhs);                                           \
    }                                                                          \
    inline T operator&(T lhs, T rhs)                                           \
    {                                                                          \
        using U = Std::UnderlyingType<T>;                                      \
        return (T)((U)lhs & (U)rhs);                                           \
    }                                                                          \
    inline T operator^(T lhs, T rhs)                                           \
    {                                                                          \
        using U = Std::UnderlyingType<T>;                                      \
        return (T)((U)lhs ^ (U)rhs);                                           \
    }                                                                          \
    inline T& operator|=(T& lhs, T rhs)                                        \
    {                                                                          \
        return lhs = lhs | rhs;                                                \
    }                                                                          \
    inline T& operator&=(T& lhs, T rhs)                                        \
    {                                                                          \
        return lhs = lhs & rhs;                                                \
    }                                                                          \
    inline T& operator^=(T& lhs, T rhs)                                        \
    {                                                                          \
        return lhs = lhs ^ rhs;                                                \
    }

template <Enum E>
class Flags
{
    using U = Std::UnderlyingType<E>;

public:
    Flags() = default;

    Flags(U flags)
        : _flags(flags)
    {
    }

    Flags(E flags)
        : _flags((U)flags)
    {
    }

    Flags<E>& operator|=(Flags<E> rhs)
    {
        _flags |= rhs._flags;
        return *this;
    }

    Flags<E>& operator|=(E rhs)
    {
        _flags |= (U)rhs;
        return *this;
    }

    Flags<E>& operator&=(Flags<E> rhs)
    {
        _flags &= rhs._flags;
        return *this;
    }

    Flags<E>& operator&=(E rhs)
    {
        _flags &= (U)rhs;
        return *this;
    }

    Flags<E>& operator^=(Flags<E> rhs)
    {
        _flags ^= rhs._flags;
        return *this;
    }

    Flags<E>& operator^=(E rhs)
    {
        _flags ^= (U)rhs;
        return *this;
    }

    Flags<E> operator~() { return ~_flags; }

    U operator|(Flags<E> rhs) { return _flags | rhs._flags; }

    U operator|(U rhs) { return _flags | rhs; }

    U operator&(Flags<E> rhs) { return _flags & rhs._flags; }

    U operator&(U rhs) { return _flags & rhs; }

    U operator^(Flags<E> rhs) { return _flags ^ rhs._flags; }

    U operator^(U rhs) { return _flags ^ rhs; }

    U operator()() { return _flags; }

    bool operator==(Flags<E> rhs) { return _flags == rhs._flags; }

    bool operator==(E rhs) { return _flags == (U)rhs; }

    bool operator!=(Flags<E> rhs) { return _flags != rhs._flags; }

    bool operator!=(E rhs) { return _flags != (U)rhs; }

    bool has(E flag) { return _flags & (U)flag; }

    bool hasNot(E flag) { return !(_flags & (U)flag); }

    void set(E flag) { _flags |= (U)flag; }

    void clear(E flag) { _flags &= ~(U)flag; }

private:
    U _flags;
};