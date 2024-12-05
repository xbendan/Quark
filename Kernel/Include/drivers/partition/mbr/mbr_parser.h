#include <mixins/meta/opt.h>
#include <mixins/meta/ref.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>

namespace MBR {
    using Qk::NonnullRefPtr;
    using Quark::System::Io::StorageDevice;
    using Quark::System::Io::FileSystem::PartitionTable;

    /**
     * @brief
     *
     * @return PartitionTable*
     */
    Optional<PartitionTable*> EnumeratePartitions(NonnullRefPtr<StorageDevice>);
}