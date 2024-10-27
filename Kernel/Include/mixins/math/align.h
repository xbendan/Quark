#pragma once

#include <mixins/std/type_traits.h>

namespace Math {
    // Align the nearest _lower_ aligned address
    // ex: 8 with align = 8 -> 8
    // ex: 9 with align = 8 -> 16
    // ex: 7 with align = 8 -> 0
    template <typename T, typename U = T>
        requires(Std::isIntegral<T> && Std::isIntegral<U>)
    static inline T AlignDown(T _addr, U _align)
    {
        return (_addr) & ~(_align - 1);
    }

    // Align the nearest _upper_ aligned address
    // ex: 8 with align = 8 -> 8
    // ex: 9 with align = 8 -> 16
    // ex: 7 with align = 8 -> 8
    template <typename T, typename U = T>
        requires(Std::isIntegral<T> && Std::isIntegral<U>)
    static inline T AlignUp(T _addr, U _align)
    {
        return ((_addr + _align) - 1) & ~(_align - 1);
    }

    template <typename T, typename U = T>
        requires(Std::isIntegral<T> && Std::isIntegral<U>)
    static inline T AlignDownRef(T& _addr, U _align)
    {
        return (_addr = AlignDown(_addr, _align));
    }

    template <typename T, typename U = T>
        requires(Std::isIntegral<T> && Std::isIntegral<U>)
    static inline T AlignUpRef(T& _addr, U _align)
    {
        return (_addr = AlignUp(_addr, _align));
    }

    template <typename T>
        requires(Std::isIntegral<T>)
    static inline void AlignTwoExponent(T& x)
    {
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        x++;
    }
}
