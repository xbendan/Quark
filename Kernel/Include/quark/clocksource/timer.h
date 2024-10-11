#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/std/time.h>
#include <mixins/utils/date.h>
#include <mixins/utils/linked_list.h>
#include <mixins/utils/singleton.h>
#include <quark/clocksource/timer_alarm.h>

namespace Quark::System {
    using Std::TimeSpan;
    enum class TimerType
    {
        RTC,
        PIT,
        HPET,
        APIC,
    };

    class Timer
    {
    public:
        virtual void            Sleep(u64)                          = 0;
        virtual void            SleepNanos(u64)                     = 0;
        virtual void            SleepUntil(Date)                    = 0;
        virtual Res<TimerAlarm> CreateAlarm(Date, Func<void()>)     = 0;
        virtual Res<TimerAlarm> CreateAlarm(TimeSpan, Func<void()>) = 0;
        virtual Date            Current()                           = 0;

    private:
        static inline Singleton<Timer*>* m_timers;
    };
}