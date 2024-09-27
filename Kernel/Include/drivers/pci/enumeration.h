#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/pci/device.h>
#include <mixins/utils/linked_list.h>

namespace PCI {
    class PCIEnumerationDevice : public Io::EnumerationDevice
    {
    public:
        PCIEnumerationDevice();

        ~PCIEnumerationDevice() = default;

        Res<> onLoad() override;

        IReadOnlyCollection<Io::Device*>* EnumerateDevices() override;

        bool     CheckDevice(u8 bus, u8 slot, u8 func);
        bool     CheckDevice(PCIInfo& info);
        PCIInfo* FindDevice(u16 deviceID, u16 vendorID);
        PCIInfo* FindGenericDevice(u8 classCode, u8 subclass);
        PCIInfo* GetDevice(u16 deviceId, u16 vendorId);
        PCIInfo* GetDevice(u8 classCode, u8 subclassCode);

    private:
        IReadOnlyCollection<Io::Device*>* m_devices;
        LinkedList<ACPI::PCIExpressSpecTable::AddressPacket*>
                                   m_enhancedBaseAddressList;
        ConfigAccessMode           m_accessMode;
        ACPI::PCIExpressSpecTable* m_pciExpressTable;
    };
} // namespace Quark::System::Hal