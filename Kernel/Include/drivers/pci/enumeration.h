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

        IReadOnlyCollection<Io::Device*>* enumerateDevices() override;

        bool     checkDevice(u8 bus, u8 slot, u8 func);
        bool     checkDevice(PCIInfo& info);
        PCIInfo* findDevice(u16 deviceID, u16 vendorID);
        PCIInfo* findGenericDevice(u8 classCode, u8 subclass);
        PCIInfo* getDevice(u16 deviceId, u16 vendorId);
        PCIInfo* getDevice(u8 classCode, u8 subclassCode);

    private:
        IReadOnlyCollection<Io::Device*>* m_devices;
        LinkedList<ACPI::PCIExpressSpecTable::AddressPacket*>
                                   m_enhancedBaseAddressList;
        ConfigAccessMode           m_accessMode;
        ACPI::PCIExpressSpecTable* m_pciExpressTable;
    };
} // namespace Quark::System::Hal