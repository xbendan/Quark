#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>

namespace Quark::System::Hal {
    enum class PmmFlags : u8
    {
        FREE    = (1 << 0),
        KERNEL  = (1 << 1),
        SWAPPED = (1 << 2),
        SHARED  = (1 << 3),
    };
    MakeFlags$(PmmFlags);

    enum class PmmType : u8
    {
        DMA = 1,
        DMA32,
        NORMAL,
    };

    Res<> InitPhysMemory();
    Res<> InitPhysAlloc();
}