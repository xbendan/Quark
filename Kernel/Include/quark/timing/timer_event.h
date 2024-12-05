#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/result.h>
#include <mixins/meta/union.h>
#include <mixins/std/utility.h>
#include <mixins/utils/date.h>

namespace Quark::System {
    using Std::TimeSpan;

    enum TimerEventInterval : u8
    {
        ONE_TIME_EVENT,
        PERIODIC_EVENT,
        CONDITIONAL_EVENT
    };

    template <TimerEventInterval interval>
    struct TimerEvent;

    template <>
    struct TimerEvent<ONE_TIME_EVENT>
    {
        Date _when;
        /*
         * @brief Callback function
         * @return true if the event is disposable after the callback
         */
        bool (*_callback)();
    };

    template <>
    struct TimerEvent<PERIODIC_EVENT>
    {
        u64      _begin; // timestamp when the event registered
        TimeSpan _interval;
        bool (*_callback)();
    };

    template <>
    struct TimerEvent<CONDITIONAL_EVENT>
    {
        TimeSpan _interval;
        bool (*_callback)();
        bool (*_condition)();
    };

    Res<TimerEvent<ONE_TIME_EVENT> const&> CreateTimerEventOneTime(
        Date,
        bool (*)());
    Res<TimerEvent<PERIODIC_EVENT> const&> CreateTimerEventPeriodically(
        TimeSpan,
        bool (*)());
    Res<TimerEvent<CONDITIONAL_EVENT> const&> CreateTimerEventConditional( //
        TimeSpan interval,
        bool (*)(),
        bool (*)());
    Res<> CancelTimerEvent(TimerEvent<ONE_TIME_EVENT> const&);
    Res<> CancelTimerEvent(TimerEvent<PERIODIC_EVENT> const&);
    Res<> CancelTimerEvent(TimerEvent<CONDITIONAL_EVENT> const&);
} // namespace Quark::System