#include <drivers/pci/info.h>
#include <drivers/pci/spec.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>
#include <mixins/str/string.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace PCI {
    using namespace Quark::System;

    class PCIDevice
        : public PCIInfo
        , public Io::Device
    {
    public:
        PCIDevice(u8 bus, u8 slot, u8 func);
        PCIDevice(u8 bus, u8 slot, u8 func, u16 vendorID, u16 deviceID);
        PCIDevice(PCIInfo& info);

        PCIDevice(PCIInfo& info, String name, Type type)
            : PCIInfo(info)
            , Io::Device(name, type)
        {
        }
        PCIDevice(PCIDevice const& other)
            : PCIInfo(other)
            , Io::Device(other.m_name, other.m_deviceType)
        {
            _bus  = other._bus;
            _slot = other._slot;
            _func = other._func;
        }
        ~PCIDevice() = default;

        PCIDevice& operator=(PCIDevice const& other)
        {
            if (this != &other) {
                _bus  = other._bus;
                _slot = other._slot;
                _func = other._func;
            }
            return *this;
        }

        PCIDevice& operator=(PCIDevice&& other)
        {
            if (this != &other) {
                _bus  = other._bus;
                _slot = other._slot;
                _func = other._func;
            }
            return *this;
        }

        bool operator==(PCIDevice const& other)
        {
            return _bus == other._bus && _slot == other._slot &&
                   _func == other._func;
        }

        u64 GetBaseAddrRegs(u8 i);

        // clang-format off

        inline bool     BarIsIoPort(u8 i)               { return Read<u32>((u8)PCI::ConfigRegs::BAR0 + i * 4) & 0x1; }
        inline u8       GetInterruptLine()              { return Read<u8>(PCI::ConfigRegs::InterruptLine); }
        inline void     SetInterruptLine(u8 value)      { Write<u8>(PCI::ConfigRegs::InterruptLine, value); }
        inline u8       GetInterruptPin()               { return Read<u8>(PCI::ConfigRegs::InterruptPin); }
        inline void     SetInterruptPin(u8 value)       { Write<u8>(PCI::ConfigRegs::InterruptPin, value); }
        inline u8       GetCommand()                    { return Read<u8>(PCI::ConfigRegs::Command); } 
        inline void     SetCommand(u8 value)            { Write<u8>(PCI::ConfigRegs::Command, value); }
        inline u8       GetStatus()                     { return Read<u8>(PCI::ConfigRegs::Status); }
        inline u8       GetHeaderType()                 { return Read<u8>(PCI::ConfigRegs::HeaderType); }
        inline void     EnableBusMastering()            { SetCommand(GetCommand() | PCI_CMD_BUS_MASTER); }
        inline void     EnableInterrupts()              { SetCommand(GetCommand() & (~PCI_CMD_INTERRUPT_DISABLE)); }
        inline void     EnableMemorySpace()             { SetCommand(GetCommand() | PCI_CMD_MEMORY_SPACE); }
        inline void     EnableIO()                      { SetCommand(GetCommand() | PCI_CMD_IO_SPACE); }

        // clang-format on

    private:
        inline u32 address(u8 offset)
        {
            return (_bus << 16) | (_slot << 11) | (_func << 8) |
                   (offset & 0xfc) | ((u32)0x80000000);
        }
    };
} // namespace Quark::System::Hal