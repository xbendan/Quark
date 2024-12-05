#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>
#include <quark/fs/partition_definition.h>

namespace GPT {
    using namespace Quark::System;
    using Qk::NonnullRefPtr;
    using Qk::Result;

    struct PartitionTableDefinition : Io::FileSystem::PartitionTableDefinition
    {
        virtual Res<NonnullRefPtr<Io::FileSystem::PartitionTable>> Read(
            NonnullRefPtr<Io::StorageDevice>) final;

        virtual Res<> Write(
            NonnullRefPtr<Io::StorageDevice>,
            NonnullRefPtr<Io::FileSystem::PartitionTable>) final;

        virtual Res<> Erase(NonnullRefPtr<Io::StorageDevice>) final;

        virtual Res<> Format(NonnullRefPtr<Io::StorageDevice>) final;
    };
}