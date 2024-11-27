#include <mixins/meta/result.h>
#include <quark/dev/storage/storage_device.h>

namespace Quark::System::Io {
    class StorageControllerDevice
    {
    public:
        virtual Res<NonnullRefPtr<StorageDevice>> GetStorageDevice(u8) = 0;
    };
}