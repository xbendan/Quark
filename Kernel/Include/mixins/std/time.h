#pragma once

#include <mixins/std/c++types.h>

namespace Std {
    enum class TimeUnit : u8
    {
        Nanosecond = 0,
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

    struct TimeSpan
    {
        u32      _amount;
        TimeUnit _unit;

        constexpr inline u64 GetAsTimestamp()
        {
            switch (_unit) {
                case TimeUnit::Nanosecond:
                    return _amount / 1000000;
                case TimeUnit::Microsecond:
                    return _amount / 1000;
                case TimeUnit::Millisecond:
                    return _amount;
                case TimeUnit::Second:
                    return _amount * 1000;
                case TimeUnit::Minute:
                    return _amount * 1000 * 60;
                case TimeUnit::Hour:
                    return _amount * 1000 * 60 * 60;
                case TimeUnit::Day:
                    return _amount * 1000 * 60 * 60 * 24;
                case TimeUnit::Week:
                    return _amount * 1000 * 60 * 60 * 24 * 7;
                case TimeUnit::Month:
                    return _amount * 1000 * 60 * 60 * 24 * 30;
                case TimeUnit::Year:
                    return _amount * 1000 * 60 * 60 * 24 * 365;
                default:
                    return 0;
            }
        }
    };
}