#include <drivers/comm/device.h>
#include <platforms/x86_64/cpu.h>
#include <quark/hal/ports.h>

namespace Serial {
    using namespace Quark::System::Hal;

    SerialPortDevice::SerialPortDevice()
        : Device("Serial Port Device", Type::Firmware)
    {
        PortAccess<>::Out<u8>(Port::COM1 + PortOffset::InterruptEnable, 0x00);
        PortAccess<>::Out<u8>(Port::COM1 + PortOffset::LineControl, 0x80);
        m_portAccess << 0x03;
        m_portAccess << 0x00;
        PortAccess<>::Out<u8>(Port::COM1 + PortOffset::LineControl, 0x03);
        PortAccess<>::Out<u8>(Port::COM1 + PortOffset::FIFO, 0xC7);
        PortAccess<>::Out<u8>(Port::COM1 + PortOffset::ModemControl, 0x0B);
    }

    usize SerialPortDevice::Write(U c)
    {
        ScopedLock lock(m_lock);
        while (!(m_lineStatus.In<u8>() & 0x20))
            ;
        m_portAccess << c;

        return 1;
    }

    usize SerialPortDevice::Write(StringView str)
    {
        ScopedLock lock(m_lock);
        usize      len = 0;
        for (char c : str) {
            while (!(m_lineStatus.In<u8>() & 0x20))
                ;
            m_portAccess << c;
            len++;
        }

        return len;
    }

    usize SerialPortDevice::Write(U* str)
    {
        ScopedLock lock(m_lock);
        usize      len = 0;
        while (*str) {
            while (!(m_lineStatus.In<u8>() & 0x20))
                ;
            m_portAccess << *str++;
            len++;
        }

        return len;
    }

    usize SerialPortDevice::Write(Buf<U> const& buf)
    {
        return Write(buf.buf());
    }
}