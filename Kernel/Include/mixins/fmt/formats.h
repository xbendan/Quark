#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/string.h>

namespace fmt {
    struct NumberFormat
    {
        String<>::Unit _p;
        u8             _base;
    };

    static constexpr NumberFormat BinaryFormat      = { 'b', 2 };
    static constexpr NumberFormat OctalFormat       = { 'o', 8 };
    static constexpr NumberFormat DecimalFormat     = { 'd', 10 };
    static constexpr NumberFormat HexadecimalFormat = { 'x', 16 };
}