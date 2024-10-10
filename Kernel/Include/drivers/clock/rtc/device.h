#include <drivers/clock/rtc/spec.h>
#include <quark/clocksource/timer.h>
#include <quark/dev/device.h>

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

        u8  Get(TimestampKey k);
        u8  GetSeconds();
        u8  GetMinutes();
        u8  GetHours();
        u8  GetDay();
        u8  GetMonth();
        u16 GetYear();

        virtual void Sleep(u64) override final;
        virtual void SleepNanos(u64) override final;
        virtual void SleepUntil(Date) override final;
        virtual Date Current() override final;
    };
}