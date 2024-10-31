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
        using U = StringView::Unit;

    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        usize Write(U c) override;
        usize Write(StringView str) override;
        usize Write(U* str) override;
        usize Write(Buf<U> const& buf) override;
        void  WriteNewline() override;

        void operator<<(U u) override;
        void operator<<(Buf<U> const& buf) override;

        void dispose() override {}

    private:
        // Spinlock                                             m_lock;
        lock_t                                          m_lock;
        PortAccess<Port::COM1>                          m_portAccess;
        PortAccess<Port::COM1 + PortOffset::LineStatus> m_lineStatus;
    };
} // namespace Quark::System::Hal