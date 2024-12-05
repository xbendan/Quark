#pragma once

#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>

namespace Quark::System::Io::FileSystem {
    using Qk::NonnullRefPtr;

    struct PartitionTableDefinition
    {
        /**
         * @brief
         *
         * @exception IOException If the partition table could not be read
         * properly
         * @return Result<NonnullRefPtr<PartitionTable>>
         */
        virtual Res<NonnullRefPtr<PartitionTable>> Read(
            NonnullRefPtr<StorageDevice>) = 0;

        virtual Res<> Write(NonnullRefPtr<StorageDevice>,
                            NonnullRefPtr<PartitionTable>) = 0;

        virtual Res<> Erase(NonnullRefPtr<StorageDevice>) = 0;

        virtual Res<> Format(NonnullRefPtr<StorageDevice>) = 0;

        usize MaxPartitionCount{ 0 };
        usize MaxPartitionSize{ 0 };
        usize MaxLogicalBlockCount{ 0 };
    };
}