#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>

namespace Quark::System::Hal {
    enum class VmmFlags : u64
    {
        PRESENT = 0,
        WRITABLE,
        USER,
        WRITE_THROUGH,
        CACHE_DISABLED,
        ACCESSED,
        DIRTY,
        PAGE_SIZE,
        GLOBAL,
        DISABLE_EXECUTE
    };
    MakeFlags$(VmmFlags);
} // namespace Quark::System::Hal