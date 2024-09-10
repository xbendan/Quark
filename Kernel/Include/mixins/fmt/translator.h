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
        requires Std::isIntegral<T>
    class Translator<string::Unit, T>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = T;

        // static constexpr bool isImplemented = true;

        static void toUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val,
                               NumberFormat                 format,

                               bool withPrefix      = false,
                               bool withCapitalized = false)
        {
            if (withPrefix) {
                stream.out('0');
                stream.out(format._p);
            }

            do {
                u8       digit = val % format._base;
                UnitType u     = (digit < 10)
                                     ? digit + '0'
                                     : digit - 10 + (withCapitalized ? 'A' : 'a');
                stream.out(u);
                val /= format._base;
            } while (val > 0);
        }

        static T toParameterType() { return {}; }
    };

    template <>
    class Translator<string::Unit, string>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = string;

        static void toUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val)
        {
            stream.out(val.data(), val.size());
        }

        static string toParameterType() { return {}; }
    };

    template <>
    class Translator<string::Unit, const char*>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = const char*;

        static void toUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val)
        {
            string str(val);
            stream.out(str.data(), str.size());
        }

        static const char* toParameterType() { return {}; }
    };

    template <typename T>
    class Translator<string::Unit, T*>
    {
    public:
        using UnitType      = string::Unit;
        using ParameterType = T*;

        static void toUnitType(OutputStream<UnitType> auto& stream,
                               ParameterType const&         val)
        {
            Translator<string::Unit, u64>::toUnitType(
                stream, reinterpret_cast<u64>(val), HexadecimalFormat, true);
        }

        static T* toParameterType() { return nullptr; }
    };

    template <typename T>
    concept Translatable = requires(T t) {
        typename Translator<string::Unit, T>::UnitType;
        typename Translator<string::Unit, T>::ParameterType;
    };

    static_assert(Translatable<string>);
}