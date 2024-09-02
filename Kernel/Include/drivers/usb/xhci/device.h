#include <drivers/usb/xhci/spec.h>

namespace Quark::System::Hal {
    class XHCIControllerDevice : public PCIDevice
    {
    public:
        XHCIControllerDevice(PCIInfo& info);
        ~XHCIControllerDevice();
    };
} // namespace Quark::System::Hal