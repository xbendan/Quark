#include <drivers/acpi/device.h>
#include <drivers/cmos/timer_device.h>
#include <quark/hal/ports.h>

namespace CMOS {
    using namespace Quark::System::Hal;

    Res<> RealTimeClockDevice::OnInitialize()
    {
        auto* fadt = Device::FindByName<ACPI::ControllerDevice>(
                         ACPI::ControllerDevice::Name)
                         .Select<ACPI::FixedAcpiDescTable*>(
                             [](ACPI::ControllerDevice* acpi) {
                                 return acpi ? acpi->TblFadt : nullptr;
                             })
                         .Take();
        assert(fadt != nullptr);

        m_centuryRegsiter = fadt->_century;
        // TODO: Log

        return Ok();
    }

    bool RealTimeClockDevice::IsUpdateInProgress()
    {
        m_indexAccess << RTC_STATUS_REG_A;
        return m_dataAccess.In<u8>() & 0x80;
    }

    u8 RealTimeClockDevice::Get(int k)
    {
        m_indexAccess << (u8)k;
        return m_dataAccess.In<u8>();
    }

    u8 RealTimeClockDevice::GetSeconds()
    {
        u8 val = Get(RTC_TIME_SECONDS);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetMinutes()
    {
        u8 val = Get(RTC_TIME_MINUTES);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetHours()
    {
        u8 val = Get(RTC_TIME_HOURS);
        return ((val & 0x0F) + ((val / 16) * 10)) | (val & 0x80);
    }

    u8 RealTimeClockDevice::GetDay()
    {
        u8 val = Get(RTC_TIME_DAY_OF_MONTH);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetMonth()
    {
        u8 val = Get(RTC_TIME_MONTH);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u16 RealTimeClockDevice::GetYear()
    {
        u8 val = Get(RTC_TIME_YEAR);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    void RealTimeClockDevice::ReadTime()
    {
        m_seconds = GetSeconds();
        m_minutes = GetMinutes();
        m_hours   = GetHours();
        m_day     = GetDay();
        m_month   = GetMonth();
        m_year    = GetYear();
        if (m_centuryRegsiter) {
            m_cen = Get(m_centuryRegsiter);
        }
    }

    void RealTimeClockDevice::Sleep(u64 ms) {}

    void RealTimeClockDevice::SleepNanos(u64 ms) {}

    void RealTimeClockDevice::SleepUntil(Date date) {}

    Res<TimerAlarm> RealTimeClockDevice::CreateAlarm(Date         date,
                                                     Func<void()> func)
    {
        return Error::NotImplemented();
    }

    Res<TimerAlarm> RealTimeClockDevice::CreateAlarm(TimeSpan     span,
                                                     Func<void()> func)
    {
        return Error::NotImplemented();
    }

    Date RealTimeClockDevice::GetToday()
    {
        u8 cen, year, month, day, hours, minutes, seconds;
        u8 regb;

        while (IsUpdateInProgress())
            ReadTime();

        do {
            cen     = m_cen;
            year    = m_year;
            month   = m_month;
            day     = m_day;
            hours   = m_hours;
            minutes = m_minutes;
            seconds = m_seconds;

            while (IsUpdateInProgress())
                ReadTime();
        } while (cen != m_cen || year != m_year || month != m_month ||
                 day != m_day || hours != m_hours || minutes != m_minutes ||
                 seconds != m_seconds);

        regb = Get(RTC_STATUS_REG_B);

        if (!(regb & 0x04)) {
            seconds = (seconds & 0x0F) + ((seconds / 16) * 10);
            minutes = (minutes & 0x0F) + ((minutes / 16) * 10);
            hours   = ((hours & 0x0F) + ((hours / 16) * 10)) | (hours & 0x80);
            day     = (day & 0x0F) + ((day / 16) * 10);
            month   = (month & 0x0F) + ((month / 16) * 10);
            year    = (year & 0x0F) + ((year / 16) * 10);
            if (cen) {
                cen = (cen & 0x0F) + ((cen / 16) * 10);
            }
        }

        if (!(regb & 0x02) && (hours & 0x80)) {
            hours = ((hours & 0x7F) + 12) % 24;
        }

        u16 fullyear = (u16)cen * 100 + year;
        return Date(fullyear, month, day, hours, minutes, seconds, 0);
    }

    u64 RealTimeClockDevice::GetSystemUptime()
    {
        return 0;
    }

    u64 RealTimeClockDevice::GetTimestamp()
    {
        return GetToday().GetAsTimestamp();
    }
}