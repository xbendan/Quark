#pragma once

#include <mixins/fmt/formats.h>
#include <mixins/fmt/string_buffer.h>
#include <mixins/io/stream.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/std/string.h>
#include <mixins/std/type_traits.h>

namespace fmt {
    template <typename TC, class T>
    class Translator
    {
        // public:
        //     using UnitType      = TC;
        //     using ParameterType = T;

        //     static constexpr bool isImplemented = false;
    };

    template <typename T>
        requires Std::isIntegral<Std::RemoveCvRef<T>>
    class Translator<string::Unit, T>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = T;

        // static constexpr bool isImplemented = true;

        template <typename... Args>
        static void ToUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType&               val,

                               NumberFormat format          = DecimalFormat,
                               bool         withPrefix      = false,
                               bool         withCapitalized = false)
        {
            if (withPrefix) {
                stream << '0';
                stream << format._p;
            }

            do {
                u8       digit = val % format._base;
                UnitType u     = (digit < 10)
                                     ? digit + '0'
                                     : digit - 10 + (withCapitalized ? 'A' : 'a');
                stream << u;
                val /= format._base;
            } while (val > 0);
        }

        static T ToParameterType() { return {}; }
    };

    template <>
    class Translator<string::Unit, string>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = string;

        static void ToUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val)
        {
            stream.out(val.Data(), val.Length());
        }

        static string ToParameterType() { return {}; }
    };

    template <>
    class Translator<string::Unit, const char*>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = const char*;

        static void ToUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val)
        {
            string str(val);
            stream.out(str.Data(), str.Length());
        }

        static const char* ToParameterType() { return {}; }
    };

    template <typename T>
    class Translator<string::Unit, T*>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = T*;

        static void ToUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType&               val)
        {
            Translator<string::Unit, u64>::ToUnitType(
                stream, reinterpret_cast<u64&>(val), HexadecimalFormat, true);
        }

        static T* ToParameterType() { return nullptr; }
    };

    template <typename T>
    concept Translatable = requires(T t) {
        typename Translator<string::Unit, T>::UnitType;
        typename Translator<string::Unit, T>::ParameterType;
    };

    static_assert(Translatable<string>);
}