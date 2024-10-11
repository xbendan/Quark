#include <drivers/clocksource/rtc/spec.h>
#include <drivers/cmos/spec.h>
#include <quark/clocksource/timer.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace RTC {
    using namespace Quark::System;

    class RealTimeClockDevice
        : public Timer
        , public Io::Device
    {
    public:
        RealTimeClockDevice()
            : Device(u8"RTC", Device::Type::TimerOrClock)
        {
        }

        bool IsUpdateInProgress();
        void ReadTime();
        u8   Get(int k);
        u8   GetSeconds();
        u8   GetMinutes();
        u8   GetHours();
        u8   GetDay();
        u8   GetMonth();
        u16  GetYear();

        virtual void Sleep(u64) override final;
        virtual void SleepNanos(u64) override final;
        virtual void SleepUntil(Date) override final;
        virtual Date Current() override final;

    private:
        Hal::PortAccess<CMOS_PORT_REG_INDEX> m_indexAccess;
        Hal::PortAccess<CMOS_PORT_REG_DATA>  m_dataAccess;
        u32                                  m_centuryRegsiter;

        u8 m_cen, m_year, m_month, m_day, m_hours, m_minutes, m_seconds;
    };
}