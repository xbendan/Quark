#pragma once

#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/std/time.h>
#include <mixins/utils/date.h>
#include <mixins/utils/list.h>
#include <mixins/utils/singleton.h>
#include <quark/timing/timer_event.h>

namespace Quark::System {
    using Qk::List;
    using Qk::Singleton;
    using Std::TimeSpan;

    enum class TimerType
    {
        RTC,
        PIT,
        HPET,
        APIC,
        ACPI,
    };

    class TimerSource
    {
    public:
        virtual void Sleep(u64)        = 0;
        virtual void SleepNanos(u64)   = 0;
        virtual u64  GetSystemUptime() = 0;

        static inline Singleton<TimerSource*> Global;
    };
}