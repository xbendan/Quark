#pragma once

#include <mixins/math/range.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/gfx/framebuffer.h>
#include <quark/hal/platform.h>
#include <quark/memory/address_range.h>
#include <quark/memory/memory_info.h>

namespace Quark::System {
    using Memory::MemoryConfiguration;

    struct LaunchConfiguration
    {
        u64 _bootTime;
        u64 _bootFlags;
        u64 _checksum;

        MemoryConfiguration _memory;

        Hal::Platform _platform;
    };
}