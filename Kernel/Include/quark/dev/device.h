#pragma once

#include <mixins/meta/func.h>
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

        Device(string name)
            : m_name(name)
            , m_deviceType(Type::Unknown)
        {
            // _uuid = UUID::generate();
        }

        Device(string name, Type deviceType)
            : m_name(name)
            , m_deviceType(deviceType)
        {
            // _uuid = UUID::generate();
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

        string GetName() const { return m_name; }

        UUID GetUniqueId() const { return m_uniqueId; }

        Type GetType() const { return m_deviceType; }

        static Optional<Device*>       FindByName(string name);
        static Optional<Device*>       FindByUniqueId(UUID uuid);
        static IReadOnlyList<Device*>* EnumerateDevices();
        static IReadOnlyList<Device*>* EnumerateDevices(Type type);
        static IReadOnlyList<Device*>* EnumerateDevices(
            Predicate<Device*> predicate);
        static Res<> Load(Device* device);
        static Res<> Unload(Device* device);
        static Res<> Rescan();

    protected:
        string m_name;
        UUID   m_uniqueId;
        Type   m_deviceType;
        bool   m_isStarted;
    };

    class EnumerationDevice : public Device
    {
    public:
        EnumerationDevice(string name)
            : Device(name, Type::SystemDevices)
        {
        }
        ~EnumerationDevice() = default;

        virtual IReadOnlyCollection<Device*>* EnumerateDevices() = 0;
    };
}

// namespace Quark::System::Io