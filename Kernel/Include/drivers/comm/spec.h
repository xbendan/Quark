#include <mixins/std/c++types.h>

namespace Serial {
    enum Port : u16
    {
        COM1 = 0x3F8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8,
        COM5 = 0x5F8,
        COM6 = 0x4F8,
        COM7 = 0x5E8,
        COM8 = 0x4E8
    };

    enum PortOffset : u16
    {
        InterruptEnable = 1,
        FIFO            = 2,
        LineControl     = 3,
        ModemControl    = 4,
        LineStatus      = 5,
        ModemStatus     = 6,
        Scratch         = 7
    };

    constexpr u16 operator+(Port port, PortOffset offset)
    {
        return static_cast<u16>(port) + static_cast<u16>(offset);
    }
} // namespace Serial