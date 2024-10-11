#include <mixins/concurrent/spinlock.h>
#include <mixins/io/text.h>

#include <drivers/comm/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace Serial {
    using namespace Quark::System;
    using Quark::System::Io::Device;

    class SerialPortDevice
        : public Device
        , public ::Io::TextWriter
    {
    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        void Write(string str) override;
        void WriteNewline() override;
        void out(u8 data) override;
        bool out(u8* data, usize len) override;
        void skip(usize len) override;
        void close() override;

    private:
        Spinlock                                             m_lock;
        Hal::PortAccess<Port::COM1>                          m_portAccess;
        Hal::PortAccess<Port::COM1 + PortOffset::LineStatus> m_lineStatus;
    };
} // namespace Quark::System::Hal