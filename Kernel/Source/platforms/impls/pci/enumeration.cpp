#include <drivers/acpi/device.h>
#include <drivers/pci/enumeration.h>

namespace PCI {
    using Qk::List;

    PCIEnumerationDevice::PCIEnumerationDevice()
        : Io::EnumerationDevice("PCI Device Enumerator")
        , m_enhancedBaseAddressList()
    {
    }

    Res<> PCIEnumerationDevice::OnInitialize()
    {
        auto acpi = Device::FindByName<ACPI::ControllerDevice>(
            ACPI::ControllerDevice::Name);
        assert(acpi.IsPresent(), "ACPI device not found");

        m_pciExpressTable =
            acpi.Take()->FindTable<ACPI::PCIExpressSpecTable>("MCFG");

        m_accessMode = (m_pciExpressTable != nullptr)
                           ? ConfigAccessMode::Enhanced
                           : ConfigAccessMode::Legacy;

        m_devices = EnumerateDevices();
        return Ok();
    }

    List<Io::Device*>* PCIEnumerationDevice::EnumerateDevices()
    {
        List<Io::Device*>* devices = new List<Io::Device*>();
        for (u16 i = 0; i < 256; i++) {
            for (u16 j = 0; j < 32; j++) {
                if (CheckDevice(i, j, 0)) {
                    PCIInfo info(i, j, 0);
                    devices->PushBack(new PCI::PCIDevice(info));

                    if (info.Read<>(PCI::ConfigRegs::HeaderType) & 0x80) {
                        for (int k = 1; k < 8; k++) { // Func
                            if (CheckDevice(i, j, k)) {
                                devices->PushBack(new PCI::PCIDevice(i, j, k));
                            }
                        }
                    }
                }
            }
        }
        return devices;
    }

    bool PCIEnumerationDevice::CheckDevice(u8 bus, u8 slot, u8 func)
    {
        PCIInfo info(bus, slot, func);
        return CheckDevice(info);
    }

    bool PCIEnumerationDevice::CheckDevice(PCIInfo& info)
    {
        return info.GetVendorID() != 0xFFFF;
    }

    bool PCIEnumerationDevice::FindDevice(u16 deviceID, u16 vendorID)
    {
        return m_devices->AnyMatch([&](Io::Device* dev) {
            auto devp = static_cast<PCI::PCIDevice*>(dev);
            return devp->GetVendorID() == vendorID &&
                   devp->GetDeviceID() == deviceID;
        });
    }

    bool PCIEnumerationDevice::FindGenericDevice(u8 classCode, u8 subclass)
    {
        return m_devices->AnyMatch([&](Io::Device* dev) {
            auto devp = static_cast<PCI::PCIDevice*>(dev);
            return devp->GetClass() == classCode &&
                   devp->GetSubclass() == subclass;
        });
    }

    Opt<Io::Device*> PCIEnumerationDevice::GetDevice(u16 deviceID, u16 vendorID)
    {
        return m_devices
            ->FindFirst([&](Io::Device* dev) {
                auto devp = static_cast<PCI::PCIDevice*>(dev);
                return devp->GetVendorID() == vendorID &&
                       devp->GetDeviceID() == deviceID;
            })
            .Extract();
    }

    Opt<Io::Device*> PCIEnumerationDevice::GetDevice(u8 classCode, u8 subclass)
    {
        return m_devices
            ->FindFirst([&](Io::Device* dev) {
                auto devp = static_cast<PCI::PCIDevice*>(dev);
                return devp->GetClass() == classCode &&
                       devp->GetSubclass() == subclass;
            })
            .Extract();
    }
}