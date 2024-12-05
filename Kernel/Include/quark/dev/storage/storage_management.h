#include <mixins/meta/ref.h>

namespace Quark::System::Io {
    using Qk::NonnullRefPtr;

    class StorageDevice;

    void AttachStorageDevice(NonnullRefPtr<StorageDevice> device);
    void DetachStorageDevice(NonnullRefPtr<StorageDevice> device);
}