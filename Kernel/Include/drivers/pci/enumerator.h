#include <drivers/pci/device.h>
#include <mixins/utils/linked_list.h>

namespace PCI {
    class PCIEnumerationDevice : public Io::EnumerationDevice
    {
    public:
        PCIEnumerationDevice()
            : Io::EnumerationDevice("PCI Device Enumerator")
        {
        }

        ~PCIEnumerationDevice() = default;

        Res<IReadOnlyCollection<Io::Device*>> enumerateDevices(
            ICollection<Device*>) override;
        bool     checkDevice(u8 bus, u8 slot, u8 func);
        bool     checkDevice(PCIInfo& info);
        PCIInfo* getDevice(u16 deviceId, u16 vendorId);
        PCIInfo* getDevice(u8 classCode, u8 subclassCode);

    private:
        LinkedList<PCIDevice> m_devices;
    };
} // namespace Quark::System::Hal