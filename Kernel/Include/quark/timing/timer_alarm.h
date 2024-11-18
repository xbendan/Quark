#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/utility.h>
#include <mixins/utils/date.h>

namespace Quark::System {
    class TimerAlarm
    {
    public:
        TimerAlarm(Date date, Func<void()> callback)
            : m_when(date)
            , m_callback(Std::move(callback))
        {
        }
        TimerAlarm(Date date, void (*callback)())
            : m_when(date)
            , m_callback(callback)
        {
        }
        // TimerAlarm(TimerAlarm const& other) = delete;
        TimerAlarm(TimerAlarm&& other)
            : m_when(other.m_when)
            , m_callback(Std::move(other.m_callback))
        {
        }

        Date GetDate() const { return m_when; }

        void operator()() { m_callback(); }

        constexpr bool operator==(TimerAlarm const& other) const
        {
            return m_when == other.m_when && m_callback == other.m_callback;
        }

    private:
        Date         m_when;
        Func<void()> m_callback;
    };
}