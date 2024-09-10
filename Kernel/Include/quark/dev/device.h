#pragma once

#include <mixins/meta/result.h>
#include <mixins/std/string.h>
#include <mixins/utils/array.h>
#include <mixins/utils/linked_list.h>
#include <mixins/utils/uuid.h>

namespace Quark::System::Io {
    class Device
    {
    public:
        enum class Type
        {
            Biometric,
            Bluetooth,
            DiskDrive,
            DiskController,
            DisplayAdapter,
            DisplayOutput,
            Firmware,
            HumanInterfaceDevice,
            Keyboard,
            Multimedia,
            NetworkAdapter,
            Portable,
            PointerDevice,
            Printer,
            PowerSupply,
            Processor,
            Security,
            SoftwareDevice,
            SystemDevices,
            Sensor,
            TimerOrClock,
            USBController,

            Unknown
        };

        class Enumerator;

        Device(String<Utf8> name)
            : _name(name)
            , _deviceType(Type::Unknown)
            , _id(0)
        {
            // _uuid = UUID::generate();
        }

        Device(String<Utf8> name, Type cls)
            : _name(name)
            , _deviceType(cls)
        {
            // _uuid = UUID::generate();
        }

        ~Device()
        {
            //
        }

        virtual Res<> onLoad() { return Error::NotImplemented(); }

        virtual Res<> onStartup() { return Error::NotImplemented(); }

        virtual Res<> onShutdown() { return Error::NotImplemented(); }

    protected:
        String<> _name;
        UUID     _uuid;
        Type     _deviceType;
        u64      _id;
    };

    class EnumerationDevice : public Device
    {
    public:
        EnumerationDevice(string name)
            : Device(name, Type::SystemDevices)
        {
        }
        ~EnumerationDevice() = default;

        virtual Res<Device*> getDeviceByName(string name) = 0;

        virtual Res<IReadOnlyCollection<Device*>> enumerateDevices(
            ICollection<Device*>) = 0;
    };
}

// namespace Quark::System::Io