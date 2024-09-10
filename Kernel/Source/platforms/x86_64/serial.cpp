#include <drivers/comm/device.h>
#include <platforms/x86_64/cpu.h>
#include <quark/hal/ports.h>

namespace Quark::System::Hal {
    using namespace Serial;

    SerialPortDevice::SerialPortDevice()
        : Device("Serial Port Device", Type::Firmware)
    {
        pOut<u8>(Port::COM1 + PortOffset::InterruptEnable, 0x00);
        pOut<u8>(Port::COM1 + PortOffset::LineControl, 0x80);
        pOut<u8>(Port::COM1, 0x03);
        pOut<u8>(Port::COM1, 0x00);
        pOut<u8>(Port::COM1 + PortOffset::LineControl, 0x03);
        pOut<u8>(Port::COM1 + PortOffset::FIFO, 0xC7);
        pOut<u8>(Port::COM1 + PortOffset::ModemControl, 0x0B);
    }

    void SerialPortDevice::out(u8 data)
    {
        while (!(pIn<u8>(Port::COM1 + PortOffset::LineStatus) & 0x20))
            ;
        pOut<u8>(Port::COM1, data);
    }

    bool SerialPortDevice::out(u8* data, usize len)
    {
        if (Platform::X64::checkInterrupts()) {
            m_lock.acquireIntDisable();
            while (len--) {
                while (!(pIn<u8>(Port::COM1 + PortOffset::LineStatus) & 0x20))
                    ;
                pOut<u8>(Port::COM1, *data++);
            }

            m_lock.release();
        } else
            while (len--) {
                while (!(pIn<u8>(Port::COM1 + PortOffset::LineStatus) & 0x20))
                    ;
                pOut<u8>(Port::COM1, *data++);
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