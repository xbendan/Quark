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
        enum class Class
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
            , _class(Class::Unknown)
            , _id(0)
        {
            // _uuid = UUID::generate();
        }

        Device(String<Utf8> name, Class cls)
            : _name(name)
            , _class(cls)
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

    private:
        String<> _name;
        UUID     _uuid;
        Class    _class;
        u64      _id;
    };

    class Device::Enumerator : public Device
    {
    public:
        Enumerator(String<Utf8> name)
            : Device(name, Class::SoftwareDevice)
        {
        }

        ~Enumerator() = default;

        [[nodiscard]]
        virtual Res<Array<Device*>> enumerate() = 0;
    };
}

// namespace Quark::System::Io