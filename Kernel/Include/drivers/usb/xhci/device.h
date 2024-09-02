#include <drivers/usb/xhci/spec.h>

namespace Quark::Hal {
    class XHCIControllerDevice : public PCIDevice
    {
    public:
        XHCIControllerDevice(PCIInfo& info);
        ~XHCIControllerDevice();
    };
} // namespace Quark::Hal