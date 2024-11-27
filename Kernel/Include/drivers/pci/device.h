#pragma once

#include <drivers/pci/info.h>
#include <drivers/pci/spec.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>
#include <mixins/str/string.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace PCI {
    using namespace Quark::System;

    class Device
        : public DeviceIdentifier
        , public Io::Device
    {
    public:
        Device(u8 bus, u8 slot, u8 func);
        Device(u8 bus, u8 slot, u8 func, u16 vendorID, u16 deviceID);
        Device(DeviceIdentifier const& id);

        Device(DeviceIdentifier const& id, Qk::StringView name, Type type)
            : DeviceIdentifier(id)
            , Io::Device(name, type)
        {
        }
        Device(Device const& other)
            : DeviceIdentifier(other)
            , Io::Device(other.m_name, other.m_deviceType)
        {
            _bus  = other._bus;
            _slot = other._slot;
            _func = other._func;
        }
        ~Device() = default;

        Device& operator=(Device const& other)
        {
            if (this != &other) {
                _bus  = other._bus;
                _slot = other._slot;
                _func = other._func;
            }
            return *this;
        }

        Device& operator=(Device&& other)
        {
            if (this != &other) {
                _bus  = other._bus;
                _slot = other._slot;
                _func = other._func;
            }
            return *this;
        }

        bool operator==(Device const& other)
        {
            return _bus == other._bus && _slot == other._slot &&
                   _func == other._func;
        }

        u64 GetBaseAddrRegs(u8 i);

        // clang-format off

        inline bool     BarIsIoPort(u8 i)               { return ReadDWord((PCI::ConfigRegs)((u8)PCI::ConfigRegs::BAR0 + i * 4)) & 0x1; }
        inline u8       GetInterruptLine()              { return ReadByte(PCI::ConfigRegs::InterruptLine); }
        inline void     SetInterruptLine(u8 value)      { WriteByte(PCI::ConfigRegs::InterruptLine, value); }
        inline u8       GetInterruptPin()               { return ReadByte(PCI::ConfigRegs::InterruptPin); }
        inline void     SetInterruptPin(u8 value)       { WriteByte(PCI::ConfigRegs::InterruptPin, value); }
        inline u8       GetCommand()                    { return ReadByte(PCI::ConfigRegs::COMMAND); } 
        inline void     SetCommand(u8 value)            { WriteByte(PCI::ConfigRegs::COMMAND, value); }
        inline u8       GetStatus()                     { return ReadByte(PCI::ConfigRegs::STATUS); }
        inline u8       GetHeaderType()                 { return ReadByte(PCI::ConfigRegs::HeaderType); }
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