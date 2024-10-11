#include <drivers/clocksource/pit/device.h>
#include <quark/hal/ports.h>

namespace PIT {
    PITimerDevice::PITimerDevice(u32 frequency)
        : Device(u8"Programmable Interval Timer", Device::Type::TimerOrClock)
        , m_frequency(frequency)
    {
        u32 divisor = PIT_NORMAL_FREQUENCY / frequency;
        m_commandAccess << (u8)0x36;
        m_dataAccess << (u8)(divisor & 0xFF);
        m_dataAccess << (u8)((divisor >> 8) & 0xFF);

        // Enable IRQ0
    }
}