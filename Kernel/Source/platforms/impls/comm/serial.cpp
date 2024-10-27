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

    void SerialPortDevice::operator<<(string str)
    {
        ScopedLock<true> lock(m_lock);
        for (auto c : str) {
            while (!(m_lineStatus.In<u8>() & 0x20))
                ;
            m_portAccess << c;
        }
    }

    void SerialPortDevice::operator<<(string::Unit c)
    {
        while (!(m_lineStatus.In<u8>() & 0x20))
            ;
        m_portAccess << c;
    }

    void SerialPortDevice::operator<<(string::Unit* str)
    {
        ScopedLock<true> lock(m_lock);
        while (*str) {
            while (!(m_lineStatus.In<u8>() & 0x20))
                ;
            m_portAccess << *str++;
        }
    }

    void SerialPortDevice::operator<<(Buf<string::Unit> const& buf)
    {
        ScopedLock<true> lock(m_lock);
        for (int i = 0; i < buf.Length(); i++) {
            while (!(m_lineStatus.In<u8>() & 0x20))
                ;
            m_portAccess << buf[i];
        }
    }
}