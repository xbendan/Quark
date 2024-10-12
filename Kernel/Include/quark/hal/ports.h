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
            asm volatile("in %1, %0" : "=a"(data) : "d"(TNum));
            return data;
        }

        template <Integral TNumType = u8>
        TNumType Out(TNumType data)
        {
            asm volatile("out %0, %1" : : "a"(data), "d"(TNum));
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
} // namespace Quark::System::Hal