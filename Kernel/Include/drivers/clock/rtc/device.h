#include <drivers/clock/rtc/spec.h>
#include <quark/dev/device.h>
#include <quark/time/clocksource.h>

namespace RTC {
    using namespace Quark::System;

    class RealTimeClockDevice
        : public System::IClockSource
        , public Io::Device
    {
    public:
        RealTimeClockDevice()
            : Device(u8"RTC", Device::Type::TimerOrClock)
        {
        }

        u8  get(TimestampKey k);
        u8  getSeconds();
        u8  getMinutes();
        u8  getHours();
        u8  getDay();
        u8  getMonth();
        u16 getYear();

        u64  getTicks() override;
        Date now() override;
        void sleep(u64 ms) override;
        void sleepUntil(Date date) override;
    };
}