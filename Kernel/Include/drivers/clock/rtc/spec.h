#include <mixins/std/c++types.h>

namespace RTC {
    enum TimestampKey : u8
    {
        Seconds = 0,
        Minutes = 2,
        Hours   = 4,
        Day     = 7,
        Month   = 8,
        Year    = 9,
        Century = 0x32
    };
}