#pragma once

#include <mixins/io/types.h>
#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/str/string_view.h>
#include <quark/dev/device.h>
#include <quark/dev/storage/storage_duplexable.h>
#include <quark/fs/file_access.h>
#include <quark/fs/partition.h>

namespace Quark::System::Io {
    using Qk::Array;
    using Qk::Iterable;
    using Qk::NonnullRefPtr;
    using Qk::SeekPos;
    using Qk::StringView;
    using Qk::Whence;
    using Quark::System::Io::FileSystem::PartitionTable;

    class StorageDevicePartition;

    class StorageDevice
        : public Device
        , public StorageDuplexable
    {
    public:
        StorageDevice(StringView name, usize sectorSize, u64 sectorCount)
            : Device(name, Type::StorageDrive)
            , StorageDuplexable(sectorSize)
            , m_sectorSize(sectorSize)
            , m_sectorCount(sectorCount)
        {
        }

        virtual Res<> OnInitialize() override;

        virtual Res<usize> Read(u64, Bytes) override  = 0;
        virtual Res<usize> Write(u64, Bytes) override = 0;

        List<NonnullRefPtr<StorageDevicePartition>> const& ListPartitions()
            const
        {
            return m_partitionEntries;
        }

        List<NonnullRefPtr<StorageDevicePartition>>& ListPartitions()
        {
            return m_partitionEntries;
        }

    protected:
        PartitionTable*                             m_partitionTable;
        List<NonnullRefPtr<StorageDevicePartition>> m_partitionEntries;
        usize const                                 m_sectorSize;
        u64 const                                   m_sectorCount;
    };
}