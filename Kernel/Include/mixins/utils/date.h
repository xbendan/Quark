#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/time.h>

#define YEAR0 1900
#define EPOCH_YEAR 1970
#define SECS_DAY (24L * 60L * 60L)

class Date
{
public:
    Date() = default;
    Date(u32 year,
         u8  month,
         u8  day,
         u8  hour,
         u8  minute,
         u8  second,
         u16 millisecond)
        : m_year(year)
        , m_month(month)
        , m_day(day)
        , m_hour(hour)
        , m_minute(minute)
        , m_second(second)
        , m_millisecond(millisecond)
        , m_dst(false)
    {
    }
    Date(u64 timestamp) {}
    ~Date() = default;

    always_inline constexpr bool operator==(Date const& other) const = default;

    u32 GetYear() const { return m_year; }
    u8  GetMonth() const { return m_month; }
    u8  GetDay() const { return m_day; }
    u8  GetHour() const { return m_hour; }
    u8  GetMinute() const { return m_minute; }
    u8  GetSecond() const { return m_second; }
    u16 GetMillisecond() const { return m_millisecond; }

    /**
     * @brief Get the date as a timestamp in milliseconds
     *
     * @ref https://www.cnblogs.com/pantttian/articles/12826298.html
     * @return u64
     */
    u64 GetAsTimestamp() const
    {
        u32 yy = m_year, mm = m_month, dd = m_day;
        if (0 >= (int)(mm -= 2)) {
            mm += 12;
            yy -= 1;
        }

        // clang-format off
        return (((
            (u64)(yy / 4 - yy / 100 + yy / 400 + (367 * mm / 12) + dd) +
            yy * 365 - 719499
            ) * 24 + m_hour
            ) * 60 + m_minute
            ) * 60 + m_second;
        // clang-format on
    }

    Date operator+(Std::TimeSpan& span) const
    {
        return Date::FromTimestamp(GetAsTimestamp() + span.GetAsTimestamp());
    }

    Date operator+(Date const& date) const
    {
        return Date::FromTimestamp(GetAsTimestamp() + date.GetAsTimestamp());
    }

    Date operator-(Std::TimeSpan& span) const
    {
        return Date::FromTimestamp(GetAsTimestamp() - span.GetAsTimestamp());
    }

    Date operator-(Date const& date) const
    {
        return Date::FromTimestamp(GetAsTimestamp() - date.GetAsTimestamp());
    }

    constexpr bool operator<(Date const& other) const;
    constexpr bool operator>(Date const& other) const;

    static constexpr inline bool IsLeapYear(u64 year)
    {
        return (!((year) % 4) && (((year) % 100) || !((year) % 400)));
    }

    static constexpr inline u8 GetYearSize(u64 year)
    {
        return IsLeapYear(year) ? 366 : 365;
    }

    static constexpr Date FromTimestamp(u64 timestamp)
    {
        static constexpr int daysInMonth[2][12] = {
            { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
            { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        };
        Date date;
        u64  dayc, dayno;
        int  year = EPOCH_YEAR;

        dayc  = timestamp % SECS_DAY;
        dayno = timestamp / SECS_DAY;

        date.m_second = dayc % 60;
        date.m_minute = (dayc % 3600) / 60;
        date.m_hour   = dayc / 3600;
        // weekday = (dayc + 4) % 7;

        while (dayno >= GetYearSize(year)) {
            dayno -= GetYearSize(year);
            year++;
        }

        date.m_year  = year;
        // year day = dayno
        date.m_month = 0;

        while (dayno >= daysInMonth[IsLeapYear(year)][date.m_month]) {
            dayno -= daysInMonth[IsLeapYear(year)][date.m_month];
            date.m_month++;
        }

        date.m_day = ++dayno;
        date.m_dst = false;

        return date;
    }

    static Date Now();

private:
    u32  m_year;
    u8   m_month;
    u8   m_day;
    u8   m_hour;
    u8   m_minute;
    u8   m_second;
    u16  m_millisecond;
    bool m_dst;
} __attribute__((packed));