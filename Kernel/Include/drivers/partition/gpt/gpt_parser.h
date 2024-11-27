#include <mixins/meta/opt.h>
#include <mixins/meta/ref.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>

namespace GPT {
    using Qk::NonnullRefPtr;
    using Quark::System::Io::StorageDevice;
    using Quark::System::Io::FileSystem::PartitionTable;

    /**
     * @brief
     *
     * @return Result<NonnullRefPtr<PartitionTable>>
     */
    Optional<PartitionTable*> Parse(NonnullRefPtr<StorageDevice>);
}