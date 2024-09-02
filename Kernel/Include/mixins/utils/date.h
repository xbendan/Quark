#include <mixins/std/c++types.h>
#include <mixins/std/time.h>

class Date
{
public:
    Date() = default;
    Date(u64 year,
         u64 month,
         u64 day,
         u64 hour,
         u64 minute,
         u64 second,
         u64 millisecond)
        : m_year(year)
        , m_month(month)
        , m_day(day)
        , m_hour(hour)
        , m_minute(minute)
        , m_second(second)
        , m_millisecond(millisecond)
    {
    }
    ~Date() = default;

    [[gnu::always_inline]]
    constexpr bool operator==(Date const& other) const = default;

    u64 getYear() const { return m_year; }
    u64 getMonth() const { return m_month; }
    u64 getDay() const { return m_day; }
    u64 getHour() const { return m_hour; }
    u64 getMinute() const { return m_minute; }
    u64 getSecond() const { return m_second; }
    u64 getMillisecond() const { return m_millisecond; }

private:
    u64 m_year;
    u64 m_month;
    u64 m_day;
    u64 m_hour;
    u64 m_minute;
    u64 m_second;
    u64 m_millisecond;
};