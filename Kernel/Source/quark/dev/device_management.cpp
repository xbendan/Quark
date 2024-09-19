#include <mixins/std/utility.h>
#include <mixins/utils/array_list.h>
#include <mixins/utils/linked_list.h>
#include <quark/api/device.h>
#include <quark/dev/connectivity.h>

namespace Quark::System::Io {
    IReadOnlyList<Device*>* Connectivity::listAll()
    {
        return new ArrayList<Device*>(m_devices);
    }

    IReadOnlyList<Device*>* Connectivity::listAll(Device::Type type)
    {
        return reinterpret_cast<IReadOnlyList<Device*>*>(&(m_devices.takeWhile(
            [&](Device* device) { return device->getType() == type; })));
    }

    IReadOnlyList<Device*>* Connectivity::listAll(Predicate<Device*> predicate)
    {
        return reinterpret_cast<IReadOnlyList<Device*>*>(
            &(m_devices.takeWhile(Std::move(predicate))));
    }

    Res<> Connectivity::addDevice(Device* device)
    {
        if (m_devices.anyMatch([&](Device* d) {
                return d->getUniqueId() == device->getUniqueId();
            })) {
            return Error::DeviceDuplicated();
        }

        m_devices += device;
        return Ok();
    }

    Res<> Connectivity::removeDevice(Device* device)
    {
        if (!m_devices.remove(device)) {
            return Error::DeviceNotFound();
        }

        return Ok();
    }

    Opt<Device*> Connectivity::findByName(string name)
    {
        return m_devices
            .findFirst(
                [&](Device* device) { return device->getName() == name; })
            .map<Device*>([](Device*& device) { return device; });
    }

    Opt<Device*> Connectivity::findByUUID(UUID uuid)
    {
        return m_devices.findFirst(
            [&](Device* device) { return device->getUniqueId() == uuid; });
    }
}