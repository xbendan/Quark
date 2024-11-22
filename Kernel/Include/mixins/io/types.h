#pragma once

#include <mixins/std/c++types.h>

namespace Qk {
    enum struct Whence
    {
        BEGIN,
        CURRENT,
        END,
    };

    struct SeekPos
    {
        Whence _whence;
        isize  _offset;
    };
}