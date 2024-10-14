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

    void SerialPortDevice::Write(string str)
    {
        static_assert(Std::isSame<string::Unit, u8>);
    }

    void SerialPortDevice::WriteNewline()
    {
        out('\n');
    }

    void SerialPortDevice::out(u8 data)
    {
        while (!(m_lineStatus.In<u8>() & 0x20))
            ;
        m_portAccess << data;
    }

    bool SerialPortDevice::out(u8* data, usize len)
    {
        if (Platform::X64::CheckInterrupts()) {
            m_lock.AcquireIntDisable();
            while (len--) {
                while (!(m_lineStatus.In<u8>() & 0x20))
                    ;
                m_portAccess << *data++;
            }

            m_lock.Release();
        } else
            while (len--) {
                while (!(m_lineStatus.In<u8>() & 0x20))
                    ;
                m_portAccess << *data++;
            }
        return true;
    }

    void SerialPortDevice::skip(usize len)
    {
        // do nothing
    }

    void SerialPortDevice::close()
    {
        // do nothing
    }
}