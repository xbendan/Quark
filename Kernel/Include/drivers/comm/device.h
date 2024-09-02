#include <mixins/concurrent/spinlock.h>
#include <mixins/io/stream.h>

#include <drivers/comm/spec.h>
#include <quark/dev/device.h>

namespace Quark::Hal {
    using Quark::Io::Device;

    class SerialPortDevice : public Device
    {
    public:
        SerialPortDevice();
        ~SerialPortDevice() = default;

        void out(u8 data);
        bool out(u8* data, usize len);
        void skip(usize len);
        void close();

    private:
        Spinlock m_lock;
    };

    static_assert(OutputStream<SerialPortDevice>);
} // namespace Quark::Hal