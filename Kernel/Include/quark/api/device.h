#include <mixins/meta/opt.h>
#include <mixins/std/type_traits.h>
#include <quark/dev/device.h>

namespace Quark::System::API {
    using Quark::System::Io::Device;

    // template <typename T = Device>
    //     requires(Std::isDerived<T, Device>)
    // Opt<T*> getRegisteredDevice(UUID uuid);

    Optional<Device*> getRegisteredDevice(string name);

    template <typename T>
        requires(Std::isDerived<Device, T>)
    Optional<T*> getRegisteredDevice(string name)
    {
        return getRegisteredDevice(name).Select<T*>(
            [](Device* device) { return static_cast<T*>(device); });
    }

    ICollection<Device*>* listDevices();
    ICollection<Device*>* listDevices(Device::Type type);

    Res<> registerDevice(Device* device);
    Res<> unregisterDevice(Device* device);
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::System::API;
#endif