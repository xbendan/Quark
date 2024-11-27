#include <drivers/usb/xhci/spec.h>

namespace Quark::System::Hal {
    class XHCIControllerDevice : public PCI::Device
    {
    public:
        XHCIControllerDevice(PCI::DeviceIdentifier& info);
        ~XHCIControllerDevice();
    };
} // namespace Quark::System::Hal