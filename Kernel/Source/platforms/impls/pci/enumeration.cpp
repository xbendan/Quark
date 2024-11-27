#include <drivers/acpi/device.h>
#include <drivers/pci/enumeration.h>
#include <quark/os/diagnostic/logging.h>

namespace PCI {
    using namespace Quark::System::Diagnostic;
    using Qk::List;

    // constexpr const char ClassNames[][0xFF + 1] = {
    //     [0x00] = "Unclassified",
    //     [0x01] = "Storage",
    //     [0x02] = "Network",
    //     [0x03] = "Display",
    //     [0x04] = "Multimedia",
    //     [0x05] = "Memory",
    //     [0x06] = "Bridge",
    //     [0x07] = "Communication",
    //     [0x08] = "Peripheral",
    //     [0x09] = "Input Device",
    //     [0x0A] = "Docking Station",
    //     [0x0B] = "Processor",
    //     [0x0C] = "Serial Bus",
    //     [0x0D] = "Wireless Controller",
    //     [0x0E] = "Intelligent Controller",
    //     [0x0F] = "Satellite Communication",
    //     [0x10] = "Encryption",
    //     [0x11] = "Signal Processing",
    // };

    PortAccess<PCI_CONFIG_ADDRESS> ConfigAddress;
    PortAccess<PCI_CONFIG_DATA>    ConfigData;

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
        for (auto* device : *m_devices)
            Device::Load(device);

        return Ok();
    }

    void PCIEnumerationDevice::EnumerateSingleFuncDevice(
        List<Io::Device*>* devices,
        u8                 bus,
        u8                 slot)
    {
        DeviceIdentifier pciInfo(bus, slot, 0);
        if (!CheckDevice(pciInfo)) {
            return;
        }

        DeviceIdentifier ident(bus, slot, 0);
        info$("[PCI] Found PCI device at {:#u}:{:#u}:00 with Class: "
              "{:#X}, Subclass: {:#X}",
              bus,
              slot,
              ident.GetClass(),
              ident.GetSubclass());
        auto* device = CreateEnumeratedDevice(ident);
        devices->PushBack(device);

        EnumerateMultipleFuncDevices(devices, bus, slot);
    }

    void PCIEnumerationDevice::EnumerateMultipleFuncDevices(
        List<Io::Device*>* devices,
        u8                 bus,
        u8                 slot)
    {
        DeviceIdentifier pciInfo(bus, slot, 0);
        if (!(pciInfo.ReadByte(PCI::ConfigRegs::HeaderType) & 0x80)) {
            return;
        }

        info("[PCI] A multi-function device bit was checked, enumerating other "
             "functions...");
        for (u8 k = 1; k < 8; k++) {
            DeviceIdentifier ident(bus, slot, k);
            if (CheckDevice(ident)) {
                info$("[PCI]   - Found PCI device at {:#u}:{:#u}:{:#u}, Class: "
                      "{:#X}, Subclass: {:#X}",
                      bus,
                      slot,
                      k,
                      ident.GetClass(),
                      ident.GetSubclass());

                auto* device = CreateEnumeratedDevice(ident);
                devices->PushBack(device);
            }
        }
    }

    List<Io::Device*>* PCIEnumerationDevice::EnumerateDevices()
    {
        List<Io::Device*>* devices = new List<Io::Device*>();
        for (u16 i = 0; i < 256; i++)
            for (u16 j = 0; j < 32; j++)
                EnumerateSingleFuncDevice(devices, i, j);

        info$("[PCI] Enumerated {} PCI devices", devices->Count());
        return devices;
    }

    bool PCIEnumerationDevice::CheckDevice(u8 bus, u8 slot, u8 func)
    {
        DeviceIdentifier deviceInfo(bus, slot, func);
        return CheckDevice(deviceInfo);
    }

    bool PCIEnumerationDevice::CheckDevice(DeviceIdentifier& info)
    {
        return info.GetVendorID() && info.GetVendorID() != 0xFFFF;
    }

    bool PCIEnumerationDevice::FindDevice(u16 deviceID, u16 vendorID)
    {
        return m_devices->AnyMatch([&](Io::Device* dev) {
            auto devp = static_cast<PCI::Device*>(dev);
            return devp->GetVendorID() == vendorID &&
                   devp->GetDeviceID() == deviceID;
        });
    }

    bool PCIEnumerationDevice::FindGenericDevice(u8 classCode, u8 subclass)
    {
        return m_devices->AnyMatch([&](Io::Device* dev) {
            auto devp = static_cast<PCI::Device*>(dev);
            return devp->GetClass() == classCode &&
                   devp->GetSubclass() == subclass;
        });
    }

    Optional<Io::Device*> PCIEnumerationDevice::GetDevice(u16 deviceID,
                                                          u16 vendorID)
    {
        return m_devices
            ->FindFirst([&](Io::Device* dev) {
                auto devp = static_cast<PCI::Device*>(dev);
                return devp->GetVendorID() == vendorID &&
                       devp->GetDeviceID() == deviceID;
            })
            .Extract();
    }

    Optional<Io::Device*> PCIEnumerationDevice::GetDevice(u8 classCode,
                                                          u8 subclass)
    {
        return m_devices
            ->FindFirst([&](Io::Device* dev) {
                auto devp = static_cast<PCI::Device*>(dev);
                return devp->GetClass() == classCode &&
                       devp->GetSubclass() == subclass;
            })
            .Extract();
    }
}