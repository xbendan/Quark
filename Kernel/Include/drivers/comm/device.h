#include <mixins/concurrent/spinlock.h>
#include <mixins/io/text.h>

#include <drivers/comm/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace Serial {
    using Quark::System::Hal::PortAccess;
    using Quark::System::Io::Device;

    class SerialPortDevice final
        : public Device
        , public TextWriter
    {
    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        void Write(char c) override { m_portAccess << c; }
        void Write(string str) override { *this << str; }
        void WriteNewline() override { m_portAccess << '\n'; }

        void operator<<(string str) override;
        void operator<<(string::Unit c) override;
        void operator<<(string::Unit* str) override;
        void operator<<(Buf<string::Unit> const& buf) override;

        void Dispose() override {}

    private:
        // Spinlock                                             m_lock;
        lock_t                                          m_lock;
        PortAccess<Port::COM1>                          m_portAccess;
        PortAccess<Port::COM1 + PortOffset::LineStatus> m_lineStatus;
    };
} // namespace Quark::System::Hal