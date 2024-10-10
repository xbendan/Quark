#include <drivers/clock/rtc/device.h>
#include <quark/hal/ports.h>

namespace RTC {
    using namespace Quark::System::Hal;

    u8 RealTimeClockDevice::Get(TimestampKey k)
    {
        pOut<u8>(0x70, k);
        return pIn<u8>(0x71);
    }

    u8 RealTimeClockDevice::GetSeconds()
    {
        u8 val = Get(Seconds);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetMinutes()
    {
        u8 val = Get(Minutes);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetHours()
    {
        u8 val = Get(Hours);
        return ((val & 0x0F) + ((val / 16) * 10)) | (val & 0x80);
    }

    u8 RealTimeClockDevice::GetDay()
    {
        u8 val = Get(Day);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::GetMonth()
    {
        u8 val = Get(Month);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u16 RealTimeClockDevice::GetYear()
    {
        u8 val = Get(Year);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    Date RealTimeClockDevice::Current()
    {
        return Date(GetYear(),
                    GetMonth(),
                    GetDay(),
                    GetHours(),
                    GetMinutes(),
                    GetSeconds(),
                    0);
    }

    void RealTimeClockDevice::Sleep(u64 ms) {}

    void RealTimeClockDevice::SleepUntil(Date date) {}
}