#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/result.h>
#include <mixins/str/string.h>
#include <mixins/utils/array.h>
#include <mixins/utils/flags.h>
#include <mixins/utils/uuid.h>

#include <mixins/utils/list.h>

namespace Quark::System::Io {
    using Qk::List;
    using Qk::StringView;

    class Device
    {
    public:
        enum class Type
        {
            Biometric,
            Bluetooth,
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
            StorageDrive,
            StorageController,
            SoftwareDevice,
            SystemDevices,
            Sensor,
            TimerOrClock,
            USBController,

            Unknown
        };

        enum class Status : u32
        {
        };

        Device(StringView name)
            : Device(name, UUID::FromName(name), Type::Unknown)
        {
        }

        Device(StringView name, Type deviceType)
            : Device(name, UUID::FromName(name), deviceType)
        {
        }

        Device(StringView name, UUID uuid, Type deviceType)
            : m_name(name)
            , m_uniqueId(uuid)
            , m_deviceType(deviceType)
        {
        }

        ~Device()
        {
            //
        }

        /**
         * @brief called in the early stage of kernel bootup
         *
         * This method is called in the early stage, when most features of the
         * kernel are not yet available, except for the basic memory management,
         * the console, and the logging system.
         *
         * @return Res<>
         */
        virtual Res<> OnInitialize() { return Ok(); }

        /**
         * @brief called when most features of the kernel are available
         *
         * This method is called when most features, such as the memory manager,
         * the scheduler, and the filesystem, are available. Before entering the
         * main loop of the kernel, this method is called to complete the
         * initialization of the device. (Before entering user mode)
         *
         * @return Res<>
         */
        virtual Res<> OnStartup() { return Ok(); }

        /**
         * @brief called when the system is shutting down
         *
         * @return Res<>
         */
        virtual Res<> OnShutdown() { return Ok(); }

        StringView GetName() const { return m_name; }

        UUID GetUniqueId() const { return m_uniqueId; }

        Type GetType() const { return m_deviceType; }

        static Optional<Device*> FindByName(StringView name);

        template <typename T>
            requires Std::isDerived<Device, T>
        static Optional<T*> FindByName(StringView name)
        {
            auto device = FindByName(name);
            if (device.IsPresent()) {
                return static_cast<T*>(device.Take());
            }
            return Empty();
        }

        static Optional<Device*> FindByUniqueId(UUID uuid);
        static IList<Device*>*   EnumerateDevices();
        static IList<Device*>*   EnumerateDevices(Type type);
        static IList<Device*>*   EnumerateDevices(Predicate<Device*> predicate);
        static Res<>             Load(Device* device);
        static Res<>             Unload(Device* device);
        static Res<>             Rescan();

    protected:
        StringView m_name;
        UUID       m_uniqueId;
        Type       m_deviceType;
        bool       m_isStarted;

        static inline List<Device*> m_devices;
    };

    MakeFlags$(Device::Status);

    class EnumerationDevice : public Device
    {
    public:
        EnumerationDevice(StringView name)
            : Device(name, Type::SystemDevices)
        {
        }
        ~EnumerationDevice() = default;

        virtual List<Device*>* EnumerateDevices() = 0;
    };
}

// namespace Quark::System::Io