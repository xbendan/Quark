#include <drivers/pci/device.h>

namespace PCI {
    PCIDevice::PCIDevice(u8 bus, u8 slot, u8 func)
        : PCIInfo(bus, slot, func)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    PCIDevice::PCIDevice(u8 bus, u8 slot, u8 func, u16 vendorID, u16 deviceID)
        : PCIInfo(bus, slot, func)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    PCIDevice::PCIDevice(PCIInfo& info)
        : PCIInfo(info)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    u64 PCIDevice::getBaseAddrRegs(u8 i)
    {
        if (i > 5) {
            return 0;
        }

        u64 bar = Read<u32>((u8)PCI::ConfigRegs::BAR0 + i * 4);
        if (!(bar & 0x1) && (bar & 0x4) && (i < 5)) {
            bar |= ((u64)Read<u32>((u8)PCI::ConfigRegs::BAR0 + (i + 1) * 4))
                   << 32;
        }

        return (bar & 0x1) ? (bar & 0xFFFFFFFFFFFFFFFC)
                           : (bar & 0xFFFFFFFFFFFFFFF0);
    }
}