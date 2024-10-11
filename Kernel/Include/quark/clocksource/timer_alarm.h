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

        Date GetDate() const { return m_when; }

    private:
        Date         m_when;
        Func<void()> m_callback;
        bool         m_isCancelled{ false };
    };
}