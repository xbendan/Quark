#pragma once

#include <mixins/io/types.h>
#include <mixins/meta/ref.h>
#include <mixins/utils/array.h>
#include <quark/dev/storage/storage_duplexable.h>
#include <quark/fs/file_access.h>
#include <quark/fs/partition.h>
#include <quark/fs/partition_definition.h>

namespace Quark::System::Io {
    using Qk::Array;
    using Qk::Iterable;
    using Qk::NonnullRefPtr;
    using Qk::SeekPos;
    using Qk::Sliceable;
    using Qk::Whence;
    using Quark::System::Io::FileSystem::FileAccess;
    using Quark::System::Io::FileSystem::Partition;
    using Quark::System::Io::FileSystem::PartitionTableDefinition;

    class StorageDevice;

    class StorageDevicePartition final : public StorageDuplexable
    {
    public:
        StorageDevicePartition(StorageDevice*   device,
                               Partition const& partition)
            : StorageDuplexable(512)
            , m_device(device)
            , m_partition(partition)
        {
        }

        virtual Res<usize> Read(u64, Bytes) override;
        virtual Res<usize> Write(u64, Bytes) override;
        Res<usize>         Read(FileAccess*, SeekPos, Bytes);
        Res<usize>         Write(FileAccess*, SeekPos, Bytes);
        bool               CanRead(FileAccess*, SeekPos) const;
        bool               CanWrite(FileAccess*, SeekPos) const;

        static List<NonnullRefPtr<StorageDevicePartition>> const&
        EnumeratePartitions(NonnullRefPtr<StorageDevice>,
                            List<PartitionTableDefinition*> const&);

        Partition const& partition() const { return m_partition; }

    private:
        StorageDevice*   m_device;
        Partition const& m_partition;
    };
}