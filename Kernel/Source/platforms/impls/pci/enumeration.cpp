#include <drivers/acpi/device.h>
#include <drivers/pci/enumeration.h>
#include <mixins/utils/array_list.h>
#include <quark/api/device.h>

namespace PCI {
    PCIEnumerationDevice::PCIEnumerationDevice()
        : Io::EnumerationDevice("PCI Device Enumerator")
        , m_enhancedBaseAddressList()
    {
    }

    Res<> PCIEnumerationDevice::onLoad()
    {
        auto acpi = ::getRegisteredDevice<ACPI::ControllerDevice>(
            "ACPI Management Device");
        if (!acpi.isPresent()) {
            return Error::DeviceNotFound();
        }

        auto table = acpi.take()->findTable<ACPI::PCIExpressSpecTable>("MCFG");

        if (table.isOkay()) {
            m_pciExpressTable = table.unwrap();
            m_accessMode      = ConfigAccessMode::Enhanced;
        } else
            m_accessMode = ConfigAccessMode::Legacy;

        m_devices = enumerateDevices();
        return Ok();
    }

    IReadOnlyCollection<Io::Device*>* PCIEnumerationDevice::enumerateDevices()
    {
        ArrayList<Io::Device*>* devices = new ArrayList<Io::Device*>();
        for (u16 i = 0; i < 256; i++) {
            for (u16 j = 0; j < 32; j++) {
                if (checkDevice(i, j, 0)) {
                    PCIInfo info(i, j, 0);
                    devices->add(new PCI::PCIDevice(info));

                    if (info.read<>(PCI::ConfigRegs::HeaderType) & 0x80) {
                        for (int k = 1; k < 8; k++) { // Func
                            if (checkDevice(i, j, k)) {
                                devices->add(new PCI::PCIDevice(i, j, k));
                            }
                        }
                    }
                }
            }
        }
        return devices;
    }
}