#include <drivers/ps2/spec.h>
#include <quark/dev/device.h>

namespace Quark::System::Hal {
    class PS2LegacyControllerDevice : public Io::Device
    {
    public:
        PS2LegacyControllerDevice();
        ~PS2LegacyControllerDevice();
    };
} // namespace Quark::System::Hal