#include <mixins/concurrent/spinlock.h>
#include <mixins/io/traits.h>

#include <drivers/comm/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace Serial {
    using Quark::System::Hal::PortAccess;
    using Quark::System::Io::Device;

    class SerialPortDevice final
        : public Device
        , public Qk::TextWriter
    {
        using U = Qk::StringView::Unit;

    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        usize write(byte b) override;
        usize write(Bytes bytes) override;
        usize writeRune(Qk::Rune r) override;
        usize writeStr(Qk::StringView str) override;
        usize flush() override;

    private:
        // Spinlock                                             m_lock;
        lock_t                                          m_lock;
        PortAccess<Port::COM1>                          m_portAccess;
        PortAccess<Port::COM1 + PortOffset::LineStatus> m_lineStatus;
    };
} // namespace Quark::System::Hal