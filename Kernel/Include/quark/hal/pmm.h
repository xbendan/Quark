#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>

namespace Quark::Hal {
    enum class PmmFlags : u8
    {
        FREE = 0,
        KERNEL,
        SWAPPED,
        SHARED,
    };
    MakeFlags$(PmmFlags);
}