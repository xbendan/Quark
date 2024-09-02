#include <drivers/clock/rtc/device.h>
#include <quark/hal/ports.h>

namespace RTC {
    using namespace Quark::System::Hal;

    u8 RealTimeClockDevice::get(TimestampKey k)
    {
        pOut<u8>(0x70, k);
        return pIn<u8>(0x71);
    }

    u8 RealTimeClockDevice::getSeconds()
    {
        u8 val = get(Seconds);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::getMinutes()
    {
        u8 val = get(Minutes);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::getHours()
    {
        u8 val = get(Hours);
        return ((val & 0x0F) + ((val / 16) * 10)) | (val & 0x80);
    }

    u8 RealTimeClockDevice::getDay()
    {
        u8 val = get(Day);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u8 RealTimeClockDevice::getMonth()
    {
        u8 val = get(Month);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u16 RealTimeClockDevice::getYear()
    {
        u8 val = get(Year);
        return (val & 0x0F) + ((val / 16) * 10);
    }

    u64 RealTimeClockDevice::getTicks()
    {
        return 0;
    }

    Date RealTimeClockDevice::now()
    {
        return Date(getYear(),
                    getMonth(),
                    getDay(),
                    getHours(),
                    getMinutes(),
                    getSeconds(),
                    0);
    }

    void RealTimeClockDevice::sleep(u64 ms) {}

    void RealTimeClockDevice::sleepUntil(Date date) {}
}