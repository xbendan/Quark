#include <mixins/std/utility.h>
#include <mixins/utils/array_list.h>
#include <mixins/utils/linked_list.h>
#include <quark/api/device.h>
#include <quark/dev/connectivity.h>

namespace Quark::System::Io {
    IReadOnlyList<Device*>* Connectivity::listAll()
    {
        return static_cast<IReadOnlyList<Device*>*>(&m_devices);
    }

    IReadOnlyList<Device*>* Connectivity::listAll(Device::Type type)
    {
        return static_cast<IReadOnlyList<Device*>*>(&(m_devices.TakeWhile(
            [&](Device* device) { return device->getType() == type; })));
    }

    IReadOnlyList<Device*>* Connectivity::listAll(Predicate<Device*> predicate)
    {
        return static_cast<IReadOnlyList<Device*>*>(
            &(m_devices.TakeWhile(Std::move(predicate))));
    }

    Res<> Connectivity::addDevice(Device* device)
    {
        if (m_devices.AnyMatch([&](Device* d) {
                return d->getUniqueId() == device->getUniqueId();
            })) {
            return Error::DeviceDuplicated();
        }

        m_devices += device;
        return Ok();
    }

    Res<> Connectivity::removeDevice(Device* device)
    {
        if (!m_devices.Remove(device)) {
            return Error::DeviceNotFound();
        }

        return Ok();
    }

    Optional<Device*> Connectivity::findByName(string name)
    {
        return m_devices
            .FindFirst([&](Device* const& device) {
                return device->getName() == name;
            })
            .Take();
    }

    Optional<Device*> Connectivity::findByUUID(UUID uuid)
    {
        return m_devices
            .FindFirst(
                [&](Device* device) { return device->getUniqueId() == uuid; })
            .Take();
    }
}