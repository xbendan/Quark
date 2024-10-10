#include <mixins/std/c++types.h>
#include <mixins/utils/date.h>
#include <mixins/utils/linked_list.h>
#include <mixins/utils/singleton.h>

namespace Quark::System {
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
        virtual void Sleep(u64)       = 0;
        virtual void SleepNanos(u64)  = 0;
        virtual void SleepUntil(Date) = 0;
        virtual Date Current()        = 0;

    private:
        Singleton<Timer*> m_timers;
    };
}