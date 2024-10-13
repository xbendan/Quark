#pragma once

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

    u64 GetYear() const { return m_year; }
    u64 GetMonth() const { return m_month; }
    u64 GetDay() const { return m_day; }
    u64 GetHour() const { return m_hour; }
    u64 GetMinute() const { return m_minute; }
    u64 GetSecond() const { return m_second; }
    u64 GetMillisecond() const { return m_millisecond; }

    u64 ToMilliseconds() const;
    u64 ToSeconds() const;

    Date operator+(Std::TimeSpan const& span) const;
    Date operator+(Date const& date) const;
    Date operator-(Std::TimeSpan const& span) const;
    Date operator-(Date const& date) const;

    constexpr bool operator<(Date const& other) const;
    constexpr bool operator>(Date const& other) const;

    static constexpr bool IsLeapYear(u64 year)
    {
        return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    }

    static Date Now();

private:
    u64 m_year;
    u64 m_month;
    u64 m_day;
    u64 m_hour;
    u64 m_minute;
    u64 m_second;
    u64 m_millisecond;
};