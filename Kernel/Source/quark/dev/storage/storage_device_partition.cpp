#include <drivers/partition/gpt/gpt_parser.h>
#include <drivers/partition/mbr/mbr_parser.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/dev/storage/storage_device_partition.h>
#include <quark/fs/partition.h>

namespace Quark::System::Io {
    using Quark::System::Io::FileSystem::Partition;
    using Quark::System::Io::FileSystem::PartitionTable;

    Res<usize> StorageDevicePartition::Read(u64 offset, Bytes buffer)
    {
        return m_device->Read(m_partition.GetLBABegin(), buffer);
    }

    Res<usize> StorageDevicePartition::Write(u64 offset, Bytes buffer)
    {
        return m_device->Write(m_partition.GetLBABegin(), buffer);
    }

    List<NonnullRefPtr<StorageDevicePartition>> const&
    StorageDevicePartition::EnumeratePartitions(
        NonnullRefPtr<StorageDevice>           device,
        List<PartitionTableDefinition*> const& definitions)
    {
        PartitionTable*                              table;
        List<NonnullRefPtr<StorageDevicePartition>>& partitions =
            device->ListPartitions();

        // Do NOT parse partitions iff the device has already been parsed
        if (!partitions.IsEmpty()) {
            return partitions;
        }

        for (auto definition : definitions)
            if (auto result = definition->Read(device); result.isSuccess()) {
                table = result.unwrap().get();
                break;
            }
        if (table == nullptr) {
            return partitions;
        }

        for (Partition& partition : *table)
            partitions += new StorageDevicePartition(device.get(), partition);

        // delete table;

        return partitions;
    }
}