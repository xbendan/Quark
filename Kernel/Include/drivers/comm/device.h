#include <mixins/concurrent/spinlock.h>
#include <mixins/io/text.h>

#include <drivers/comm/spec.h>
#include <quark/dev/device.h>

namespace Quark::System::Hal {

    class SerialPortDevice
        : public Io::Device
        , public ::Io::TextWriter
    {
    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        void writeStr(string str) override;
        void writeNewline() override;
        void out(u8 data) override;
        bool out(u8* data, usize len) override;
        void skip(usize len) override;
        void close() override;

    private:
        Spinlock m_lock;
    };
} // namespace Quark::System::Hal