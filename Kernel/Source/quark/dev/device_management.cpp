#include <mixins/std/utility.h>
#include <mixins/utils/rbtree.h>
#include <mixins/utils/strings.h>
#include <quark/dev/device.h>
#include <quark/os/diagnostic/logging.h>

#include <mixins/utils/list.h>

namespace Quark::System::Io {
    using namespace Quark::System::Diagnostic;
    // RbTree<string, Device*> devices;

    Optional<Device*> Device::FindByName(Qk::StringView name)
    {
        // 实现查找设备的方法
        // 按名称查找设备的最佳方法是使用红黑树实现的表
        // 将传入的 @name 进行哈希处理，这样我们就可以
        // 利用红黑树的特性，对设备进行二分查找
        for (auto& device : m_devices) {
            Qk::StringView deviceName = device->GetName();
            if (Qk::Strings::Equals(deviceName, name)) {
                return device;
            }
        }
        return Empty();
    }

    Optional<Device*> Device::FindByUniqueId(UUID uuid)
    {
        // 实现查找设备的方法
        // 和 FindByName 方法类似，将传入的 @uuid 进行
        // 哈希处理
        return m_devices
            .FindFirst([&uuid](Device* device) {
                return device->GetUniqueId() == uuid;
            })
            .Extract();
    }

    Res<> Device::Load(Device* device)
    {
        if (!device || device->m_name.isEmpty() || device->m_uniqueId.IsNull())
            return Error::InvalidArgument("Device name or UUID is invalid");

        info$("[Device] Loading device: {}", device->GetName());
        device->OnInitialize();
        // devices.PutIfAbsent(device->GetName(), device);
        // 实现加载设备的方法
        // 将设备添加到设备表中
        m_devices.PushBack(device);
        return Ok();
    }

    Res<> Device::Unload(Device* device) {}
}