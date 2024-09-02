#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>

namespace Quark::Hal {
    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline T pIn(u16 port)
    {
        T data;
        asm volatile("in %1, %0" : "=a"(data) : "d"(port));
        return data;
    }

    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline void pOut(u16 port, T data)
    {
        asm volatile("out %0, %1" : : "a"(data), "d"(port));
    }

    static inline void waitIO()
    {
        pOut<u8>(0x80, 0);
    }
} // namespace Quark::Hal