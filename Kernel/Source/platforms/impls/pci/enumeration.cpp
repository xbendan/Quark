#include <drivers/acpi/device.h>
#include <drivers/pci/enumeration.h>
#include <mixins/utils/array_list.h>

namespace PCI {
    PCIEnumerationDevice::PCIEnumerationDevice()
        : Io::EnumerationDevice("PCI Device Enumerator")
        , m_enhancedBaseAddressList()
    {
    }

    Res<> PCIEnumerationDevice::OnInitialize()
    {
        auto acpi = Device::FindByName<ACPI::ControllerDevice>(
            "ACPI Management Device");
        if (!acpi.IsPresent()) {
            return Error::DeviceNotFound();
        }

        auto table = acpi.Take()->FindTable<ACPI::PCIExpressSpecTable>("MCFG");

        if (table.IsOkay()) {
            m_pciExpressTable = table.Unwrap();
            m_accessMode      = ConfigAccessMode::Enhanced;
        } else
            m_accessMode = ConfigAccessMode::Legacy;

        m_devices = EnumerateDevices();
        return Ok();
    }

    ICollection<Io::Device*>* PCIEnumerationDevice::EnumerateDevices()
    {
        ArrayList<Io::Device*>* devices = new ArrayList<Io::Device*>();
        for (u16 i = 0; i < 256; i++) {
            for (u16 j = 0; j < 32; j++) {
                if (CheckDevice(i, j, 0)) {
                    PCIInfo info(i, j, 0);
                    devices->Add(new PCI::PCIDevice(info));

                    if (info.read<>(PCI::ConfigRegs::HeaderType) & 0x80) {
                        for (int k = 1; k < 8; k++) { // Func
                            if (CheckDevice(i, j, k)) {
                                devices->Add(new PCI::PCIDevice(i, j, k));
                            }
                        }
                    }
                }
            }
        }
        return static_cast<IList<Io::Device*>*>(devices);
    }
}