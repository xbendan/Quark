#include <mixins/meta/opt.h>
#include <mixins/std/type_traits.h>
#include <quark/dev/device.h>

namespace Quark::System::API {
    using Quark::System::Io::Device;

    template <typename T = Device>
        requires(Std::isDerived<Device, T>)
    Opt<T*> getRegisteredDevice(string name)
    {
        return getRegisteredDevice(name).map<T*>(
            [](Device* device) { return static_cast<T*>(device); });
    }

    // template <typename T = Device>
    //     requires(Std::isDerived<T, Device>)
    // Opt<T*> getRegisteredDevice(UUID uuid);

    Opt<Device*> getRegisteredDevice(string name);

    IReadOnlyCollection<Device*>* listDevices();
    IReadOnlyCollection<Device*>* listDevices(Device::Type type);

    Res<> registerDevice(Device* device);
    Res<> unregisterDevice(Device* device);
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::System::API;
#endif