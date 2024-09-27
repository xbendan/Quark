#pragma once

#include <mixins/meta/opt.h>
#include <quark/dev/device.h>

namespace Quark::System::Io {
    class Connectivity
    {
    public:
        // Generic device api methods
        /**
         * @brief
         *
         * @return IReadOnlyList<Device*>*s
         */
        static IReadOnlyList<Device*>* listAll();

        /**
         * @brief
         *
         * @param type
         * @return IReadOnlyList<Device*>*
         */
        static IReadOnlyList<Device*>* listAll(Device::Type type);

        static IReadOnlyList<Device*>* listAll(Predicate<Device*> predicate);

        /**
         * @brief
         *
         * @param device
         * @return Res<>
         */
        static Res<> addDevice(Device* device);

        static Res<> removeDevice(Device* device);

        static Optional<Device*> findByName(string name);

        template <typename T>
            requires(Std::isDerived<Device, T>)
        static inline Optional<T*> findByName(string name)
        {
            return findByName(name).Select<T*>(
                [](Device* device) { return static_cast<T*>(device); });
        }

        static Optional<Device*> findByUUID(UUID uuid);

        template <typename T>
            requires(Std::isDerived<Device, T>)
        static inline Optional<T*> findByUUID(UUID uuid)
        {
            return findByUUID(uuid).Select<T*>(
                [](Device* device) { return static_cast<T*>(device); });
        }

    private:
        Connectivity() = delete;

        static inline LinkedList<Device*> m_devices;
    };
}