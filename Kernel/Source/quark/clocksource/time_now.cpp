#include <mixins/utils/date.h>
#include <quark/clocksource/timer.h>

Date
Date::Now()
{
    using Quark::System::Timer;
    // 实现获取当前日期的方法
    return Timer::GetClockSource()->GetToday();
}