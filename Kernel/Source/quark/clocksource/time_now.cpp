#include <mixins/utils/date.h>
#include <quark/timing/clock_source.h>

Date
Date::Now()
{
    using Quark::System::ClockSource;
    // 实现获取当前日期的方法
    return ClockSource::Global()->GetToday();
}