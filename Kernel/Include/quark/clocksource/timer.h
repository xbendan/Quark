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
        Timer(TimerType type)
            : m_type(type)
        {
            m_timerset->Add(this);
        }

        virtual void            Sleep(u64)                          = 0;
        virtual void            SleepNanos(u64)                     = 0;
        virtual void            SleepUntil(Date)                    = 0;
        virtual Res<TimerAlarm> CreateAlarm(Date, Func<void()>)     = 0;
        virtual Res<TimerAlarm> CreateAlarm(TimeSpan, Func<void()>) = 0;
        virtual Date            GetToday()                          = 0;
        virtual u64             GetSystemUptime()                   = 0;
        virtual u64             GetTimestamp()                      = 0;

        TimerType GetType() const { return m_type; }

        static inline Timer* GetClockSource()
        {
            if (m_clocksource == nullptr) {
                m_clocksource =
                    m_timerset
                        ->FindAny([](Timer* timer) {
                            return timer->GetType() == TimerType::RTC;
                        })
                        .Take();
            }
            assert(m_clocksource, "No clock source found");

            return m_clocksource;
        }

        static inline Timer* GetTimerSource()
        {
            if (m_timersource == nullptr) {
                m_timersource =
                    m_timerset
                        ->FindAny([](Timer* timer) {
                            return timer->GetType() == TimerType::PIT;
                        })
                        .Take();
            }
            assert(m_timersource, "No timer source found");

            return m_timersource;
        }

    private:
        TimerType m_type;

        static inline LinkedList<Timer*>* m_timerset = new LinkedList<Timer*>();
        static inline Timer*              m_clocksource = nullptr;
        static inline Timer*              m_timersource = nullptr;
    };
}