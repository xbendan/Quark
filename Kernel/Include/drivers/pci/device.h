#include <drivers/pci/info.h>
#include <drivers/pci/spec.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/std/type_traits.h>
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

        PCIDevice(PCIInfo& info, string name, Type type)
            : PCIInfo(info)
            , Io::Device(name, type)
        {
        }
        PCIDevice(PCIDevice const& other)
            : PCIInfo(other)
            , Io::Device(other._name, other._deviceType)
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

        u64 getBaseAddrRegs(u8 i);

        // clang-format off

        inline bool     barIsIoPort(u8 i)          { return read<u32>((u8)PCI::ConfigRegs::BAR0 + i * 4) & 0x1; }
        inline u8  getInterruptLine()              { return read<u8>(PCI::ConfigRegs::InterruptLine); }
        inline void     setInterruptLine(u8 value) { write<u8>(PCI::ConfigRegs::InterruptLine, value); }
        inline u8  getInterruptPin()               { return read<u8>(PCI::ConfigRegs::InterruptPin); }
        inline void     setInterruptPin(u8 value)  { write<u8>(PCI::ConfigRegs::InterruptPin, value); }
        inline u8  getCommand()                    { return read<u8>(PCI::ConfigRegs::Command); } 
        inline void     setCommand(u8 value)       { write<u8>(PCI::ConfigRegs::Command, value); }
        inline u8  getStatus()                     { return read<u8>(PCI::ConfigRegs::Status); }
        inline u8  getHeaderType()                 { return read<u8>(PCI::ConfigRegs::HeaderType); }
        inline void     enableBusMastering()            { setCommand(getCommand() | PCI_CMD_BUS_MASTER); }
        inline void     enableInterrupts()              { setCommand(getCommand() & (~PCI_CMD_INTERRUPT_DISABLE)); }
        inline void     enableMemorySpace()             { setCommand(getCommand() | PCI_CMD_MEMORY_SPACE); }
        inline void     enableIO()                      { setCommand(getCommand() | PCI_CMD_IO_SPACE); }

        // clang-format on

    private:
        inline u32 address(u8 offset)
        {
            return (_bus << 16) | (_slot << 11) | (_func << 8) |
                   (offset & 0xfc) | ((u32)0x80000000);
        }
    };
} // namespace Quark::System::Hal