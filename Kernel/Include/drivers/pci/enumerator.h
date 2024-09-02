#include <drivers/pci/device.h>
#include <mixins/utils/linked_list.h>

namespace Quark::System::Hal {
    class PCIDeviceEnumerator : public Io::Device::Enumerator
    {
    public:
        PCIDeviceEnumerator()
            : Io::Device::Enumerator("PCI Device Enumerator")
        {
        }

        ~PCIDeviceEnumerator() = default;

        [[nodiscard]]
        Res<Array<Device*>> enumerate() override;
        bool                checkDevice(u8 bus, u8 slot, u8 func);
        bool                checkDevice(PCIInfo& info);
        PCIInfo*            getDevice(u16 deviceId, u16 vendorId);
        PCIInfo*            getDevice(u8 classCode, u8 subclassCode);

    private:
        Io::DeviceConnectivity* m_deviceHost;
        LinkedList<PCIDevice>   m_devices;
    };
} // namespace Quark::System::Hal