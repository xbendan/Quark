#include <drivers/pci/spec.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>
#include <quark/hal/ports.h>

namespace Quark::Hal {

    class PCIInfo
    {
    public:
        template <typename _T>
            requires(Std::isSame<_T, u8> || Std::isSame<_T, u16> ||
                     Std::isSame<_T, u32>)
        _T read(u8 offset)
        {
            pOut<u32>(PCI_CONFIG_ADDRESS, getAddressPackage(offset));

            _T data = pIn<u32>(PCI_CONFIG_DATA);
            switch (sizeof(_T)) {
                case 1:
                    return data >> ((offset & 3) * 8) & 0xff;
                case 2:
                    return data >> ((offset & 2) * 8) & 0xffff;
                case 4:
                    return data;
            }
            // TODO: throw exception
        }

        template <typename _T>
        _T read(PCI::ConfigRegs reg)
        {
            return read<_T>(static_cast<u8>(reg));
        }

        template <typename _T>
            requires(Std::isSame<_T, u8> || Std::isSame<_T, u16> ||
                     Std::isSame<_T, u32>)
        void write(u8 offset, _T value)
        {
            pOut<u32>(PCI_CONFIG_ADDRESS, getAddressPackage(offset));

            _T data = pIn<u32>(PCI_CONFIG_DATA);
            switch (sizeof(_T)) {
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
            pOut<u32>(PCI_CONFIG_DATA, data);
        }

        template <typename _T>
        void write(PCI::ConfigRegs reg, _T value)
        {
            write<_T>(static_cast<u8>(reg), value);
        }

        u64 getAddressPackage(u8 offset)
        {
            return ((_bus << 16) | (_slot << 11) | (_func << 8) |
                    (offset & 0xFC)) |
                   0x80000000;
        }

        u8 getBus() { return _bus; }

        u8 getSlot() { return _slot; }

        u8 getFunc() { return _func; }

        u16 getVendorID()
        {
            return _vendorID
                       ? _vendorID
                       : (_vendorID = read<u16>(PCI::ConfigRegs::VendorID));
        }

        u16 getDeviceID()
        {
            return _deviceID
                       ? _deviceID
                       : (_deviceID = read<u16>(PCI::ConfigRegs::DeviceID));
        }

        u8 getClass()
        {
            return _class ? _class
                          : (_class = read<u8>(PCI::ConfigRegs::Class));
        }

        u8 getSubclass()
        {
            return _subclass
                       ? _subclass
                       : (_subclass = read<u8>(PCI::ConfigRegs::Subclass));
        }

        u8 getProgIF()
        {
            return _progIF ? _progIF
                           : (_progIF = read<u8>(PCI::ConfigRegs::ProgIF));
        }

        u16 _vendorID, _deviceID;
        u8  _bus, _slot, _func;
        u8  _class, _subclass, _progIF;
    };
} // namespace Quark::Hal