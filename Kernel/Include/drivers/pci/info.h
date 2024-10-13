#include <drivers/pci/spec.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>
#include <quark/hal/ports.h>

namespace PCI {
    using namespace Quark::System;
    using namespace Quark::System::Hal;

    class PCIInfo
    {
    public:
        PCIInfo() = default;
        PCIInfo(u8 bus, u8 slot, u8 func)
            : _bus(bus)
            , _slot(slot)
            , _func(func)
        {
        }

        template <Integral TNumType = u8>
        TNumType Read(u8 offset)
        {
            TNumType data;

            PortAccess<PCI_CONFIG_ADDRESS>() << (u32)GetAddressPackage(offset);
            PortAccess<PCI_CONFIG_DATA>() >> data;

            switch (sizeof(TNumType)) {
                case 1:
                    return data >> ((offset & 3) * 8) & 0xff;
                case 2:
                    return data >> ((offset & 2) * 8) & 0xffff;
                case 4:
                    return data;
            }
            // TODO: throw exception
        }

        template <Integral TNumType = u8>
        TNumType Read(PCI::ConfigRegs reg)
        {
            return Read<TNumType>(static_cast<u8>(reg));
        }

        template <Integral TNumType>
        void Write(u8 offset, TNumType value)
        {
            PortAccess<PCI_CONFIG_ADDRESS>() << (u32)GetAddressPackage(offset);

            TNumType data;
            PortAccess<PCI_CONFIG_DATA>() >> data;
            switch (sizeof(TNumType)) {
                case 1:
                    data &= ~(0xff << ((offset & 3) * 8));
                    data |= (value & 0xff) << ((offset & 3) * 8);
                    break;
                case 2:
                    data &= ~(0xffff << ((offset & 2) * 8));
                    data |= (value & 0xffff) << ((offset & 2) * 8);
                    break;
                case 4:
                    data = value;
                    break;
            }
            PortAccess<PCI_CONFIG_DATA>() << data;
        }

        template <typename TNumType>
        void Write(PCI::ConfigRegs reg, TNumType value)
        {
            Write<TNumType>(static_cast<u8>(reg), value);
        }

        inline u64 GetAddressPackage(u8 offset)
        {
            return ((_bus << 16) | (_slot << 11) | (_func << 8) |
                    (offset & 0xFC)) |
                   0x80000000;
        }

        inline u16 GetVendorID()
        {
            return _vendorID
                       ? _vendorID
                       : (_vendorID = Read<u16>(PCI::ConfigRegs::VendorID));
        }

        inline u16 GetDeviceID()
        {
            return _deviceID
                       ? _deviceID
                       : (_deviceID = Read<u16>(PCI::ConfigRegs::DeviceID));
        }

        inline u8 GetClass()
        {
            return _class ? _class
                          : (_class = Read<u8>(PCI::ConfigRegs::Class));
        }

        inline u8 GetSubclass()
        {
            return _subclass
                       ? _subclass
                       : (_subclass = Read<u8>(PCI::ConfigRegs::Subclass));
        }

        inline u8 GetProgIF()
        {
            return _progIF ? _progIF
                           : (_progIF = Read<u8>(PCI::ConfigRegs::ProgIF));
        }

        u16 _vendorID{ 0 }, _deviceID{ 0 };
        u8  _bus, _slot, _func;
        u8  _class{ 0 }, _subclass{ 0 }, _progIF{ 0 };
    };
} // namespace Quark::System::Hal