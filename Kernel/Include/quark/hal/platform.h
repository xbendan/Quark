#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/flags.h>

namespace Quark::System::Hal {
    struct PlatformDefinition
    {
        string _name;
        string _version;
        u8     _bits;
        enum Features : u8
        {
            AddressSpaceIsolation,
            ProcessContextSwitch,
        };
        Flags<Features> _features;
    };
}