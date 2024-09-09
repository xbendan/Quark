#pragma once

#include <mixins/math/range.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/hal/platform.h>
#include <quark/memory/address_range.h>
#include <quark/memory/memory_info.h>

namespace Quark::System {
    struct BootstrapConfiguration
    {};
}

struct OsBootConfig
{
    u64 _checksum;
    u64 _bootTime;
    u64 _bootFlags;

    OsMemoryConfig                         _memory;
    Quark::System::Hal::PlatformDefinition _platform;

    struct OsGraphicsInfo
    {
        u64 _width;
        u64 _height;
        u64 _address;
        u64 _depth;
        u64 _pitch;
    } _gfx;

    OsBootConfig() = default;

    OsBootConfig(Quark::System::Hal::PlatformDefinition& platform)
        : _platform(platform)
    {
        _checksum  = 0;
        _bootTime  = 0;
        _bootFlags = 0;
        // _gfx       = { 0, 0, 0, 0, 0 };
    }
};