#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/std/type_traits.h>

namespace Quark::System::Hal {
    template <u16 TNum = 0>
    class PortAccess
    {
    public:
        template <Integral TNumType = u8>
        TNumType In()
        {
            TNumType data;
            asm volatile("in %1, %0" : "=a"(data) : "dN"(TNum));
            return data;
        }

        template <Integral TNumType = u8>
        TNumType Out(TNumType data)
        {
            asm volatile("out %1, %0" : : "d"(TNum), "a"(data));
            return data;
        }

        template <Integral TNumType = u8>
        void operator<<(TNumType data)
        {
            Out(data);
        }

        template <Integral TNumType = u8>
        void operator>>(TNumType& data)
        {
            data = In<TNumType>();
        }

        template <Integral TNumType = u8>
        TNumType operator()()
        {
            return In<TNumType>();
        }

        template <Integral TNumType>
        static inline void Out(u16 port, TNumType data)
        {
            asm volatile("out %0, %1" : : "a"(data), "d"(port));
        }

        template <Integral TNumType>
        static inline TNumType In(u16 port)
        {
            TNumType data;
            asm volatile("in %1, %0" : "=a"(data) : "d"(port));
            return data;
        }
    };

    template <Integral TInputType, usize TNum, typename TNumType = TInputType>
    void operator<<(TInputType& input, PortAccess<TNum>& port)
    {
        input = port.template In<TNumType>();
    }

    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline T in(u16 port)
    {
        T data;
        asm volatile("in %1, %0" : "=a"(data) : "d"(port));
        return data;
    }

    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline void in(u16 port, T* data, usize len)
    {
        for (usize i = 0; i < len; i++) {
            data[i] = in<T>(port);
        }
    }

    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline void out(u16 port, T data)
    {
        asm volatile("out %0, %1" : : "a"(data), "d"(port));
    }

    template <typename T = u8>
        requires Std::isIntegral<T>
    static inline void out(u16 port, T* data, usize len)
    {
        for (usize i = 0; i < len; i++) {
            out<T>(port, data[i]);
        }
    }

    template <typename T = u8, bool Eq = true>
        requires Std::isIntegral<T>
    static inline void wait(u16 port, u8 bit)
    {
        T  data;
        u8 mask;
        if constexpr (Eq) {
            mask = bit;
        } else {
            mask = 0;
        }

        do {
            data = in<T>(port);
        } while ((data & bit) == mask);
    }

    static inline void waitIO()
    {
        out<u8>(0x80, 0);
    }
} // namespace Quark::System::Hal