#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/pci/device.h>
#include <mixins/utils/list.h>

namespace PCI {
    using Qk::List;

    class PCIEnumerationDevice : public Io::EnumerationDevice
    {
    public:
        PCIEnumerationDevice();

        ~PCIEnumerationDevice() = default;

        Res<> OnInitialize() override;

        List<Io::Device*>* EnumerateDevices() override;

        void EnumerateSingleFuncDevice(List<Io::Device*>*, u8 bus, u8 slot);
        void EnumerateMultipleFuncDevices(List<Io::Device*>*, u8 bus, u8 slot);
        bool CheckDevice(u8 bus, u8 slot, u8 func);
        bool CheckDevice(PCIInfo& info);
        bool FindDevice(u16 deviceID, u16 vendorID);
        bool FindGenericDevice(u8 classCode, u8 subclass);
        Opt<Device*> GetDevice(u16 deviceId, u16 vendorId);
        Opt<Device*> GetDevice(u8 classCode, u8 subclassCode);

    private:
        List<Io::Device*>* m_devices;
        List<ACPI::PCIExpressSpecTable::AddressPacket*>
                                   m_enhancedBaseAddressList;
        ConfigAccessMode           m_accessMode;
        ACPI::PCIExpressSpecTable* m_pciExpressTable;
    };
} // namespace Quark::System::Hal