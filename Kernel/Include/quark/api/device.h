#include <mixins/meta/opt.h>
#include <mixins/std/type_traits.h>
#include <quark/dev/device.h>

namespace Quark::API {
    using Quark::Io::Device;

    template <typename T = Device>
        requires(Std::isDerived<Device, T>)
    Opt<T*> getRegisteredDevice(String<> name);

    template <typename T = Device>
        requires(Std::isDerived<T, Device>)
    Opt<T*> getRegisteredDevice(UUID uuid);

    Res<> registerDevice(Device* device);
    Res<> unregisterDevice(Device* device);

    LinkedList<Device*>* enumerateRegisteredDevices();
}