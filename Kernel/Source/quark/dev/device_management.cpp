#include <mixins/std/utility.h>
#include <mixins/utils/rbtree.h>
#include <quark/dev/device.h>

#include <mixins/utils/linked_list.h>

namespace Quark::System::Io {
    // RbTree<string, Device*> devices;
    // LinkedList<Device*> Devices;

    Optional<Device*> Device::FindByName(string name)
    {
        // 实现查找设备的方法
        // 按名称查找设备的最佳方法是使用红黑树实现的表
        // 将传入的 @name 进行哈希处理，这样我们就可以
        // 利用红黑树的特性，对设备进行二分查找
        return Devices
            .FindFirst(
                [&name](Device* device) { return device->GetName() == name; })
            .Select<Device*>([](Device*& device) { return device; });
    }

    Optional<Device*> Device::FindByUniqueId(UUID uuid)
    {
        // 实现查找设备的方法
        // 和 FindByName 方法类似，将传入的 @uuid 进行
        // 哈希处理
        return Devices
            .FindFirst([&uuid](Device* device) {
                return device->GetUniqueId() == uuid;
            })
            .Select<Device*>([](Device*& device) { return device; });
    }

    Res<> Device::Load(Device* device)
    {
        if (device->m_name.IsEmpty() || device->m_uniqueId.IsNull())
            return Error::InvalidArgument("Device name or UUID is invalid");

        // devices.PutIfAbsent(device->GetName(), device);
        // 实现加载设备的方法
        // 将设备添加到设备表中
        return Ok();
    }

    Res<> Device::Unload(Device* device) {}
}