#pragma once

#include <mixins/math/range.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/gfx/framebuffer.h>
#include <quark/hal/platform.h>
#include <quark/memory/address_range.h>
#include <quark/memory/memory_info.h>

namespace Quark::System {
    using Memory::MemoryConfigTable;

    struct BootInfo
    {
        static inline u64  BootTime;
        static inline char BootloaderName[64];

        static inline MemoryConfigTable MemoryInfo;

        static inline Hal::Platform Platform;
    };
}