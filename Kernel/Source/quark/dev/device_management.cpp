#include <mixins/std/utility.h>
#include <mixins/utils/array_list.h>
#include <mixins/utils/linked_list.h>
#include <quark/dev/connectivity.h>

namespace Quark::System::Io {
    Optional<Device*> Device::FindByName(string name)
    {
        // 实现查找设备的方法
        // 按名称查找设备的最佳方法是使用红黑树实现的表
        // 将传入的 @name 进行哈希处理，这样我们就可以
        // 利用红黑树的特性，对设备进行二分查找
        return Empty();
    }

    Optional<Device*> Device::FindByUniqueId(UUID uuid)
    {
        // 实现查找设备的方法
        // 和 FindByName 方法类似，将传入的 @uuid 进行
        // 哈希处理
        return Empty();
    }
}