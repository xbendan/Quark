#include <drivers/pci/device.h>

namespace PCI {
    Device::Device(u8 bus, u8 slot, u8 func)
        : DeviceIdentifier(bus, slot, func)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    Device::Device(u8 bus, u8 slot, u8 func, u16 vendorID, u16 deviceID)
        : DeviceIdentifier(bus, slot, func)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    Device::Device(DeviceIdentifier const& info)
        : DeviceIdentifier(info)
        , Io::Device("Unknown PCI Device", Io::Device::Type::Unknown)
    {
    }

    u64 Device::GetBaseAddrRegs(u8 i)
    {
        if (i > 5) {
            return 0;
        }

        PCI::ConfigRegs reg =
            static_cast<PCI::ConfigRegs>((u8)PCI::ConfigRegs::BAR0 + i * 4);

        u64 bar = ReadDWord(reg);
        if (!(bar & 0x1) && (bar & 0x4) && (i < 5)) {
            bar |= ((u64)ReadDWord(reg)) << 32;
        }

        return (bar & 0x1) ? (bar & 0xFFFFFFFFFFFFFFFC)
                           : (bar & 0xFFFFFFFFFFFFFFF0);
    }
}