#pragma once

#include <mixins/std/c++types.h>

namespace Std {
    enum class TimeUnit
    {
        Nanosecond,
        Microsecond,
        Millisecond,
        Second,
        Minute,
        Hour,
        Day,
        Week,
        Month,
        Year,
    };

    class TimeSpan
    {};
}