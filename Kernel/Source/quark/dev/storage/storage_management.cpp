#include <quark/dev/device.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/dev/storage/storage_device_partition.h>
#include <quark/dev/storage/storage_management.h>
#include <quark/fs/partition.h>
#include <quark/fs/partition_definition.h>

namespace Quark::System::Io {
    using Quark::System::Io::FileSystem::PartitionTableDefinition;

    List<PartitionTableDefinition*> s_partitionTableDefinitions;
    List<StorageDevicePartition*>   s_partitions;

    void AttachStorageDevice(NonnullRefPtr<StorageDevice> device)
    {
        Device::Load(device.get());
        for (auto& part : StorageDevicePartition::EnumeratePartitions(
                 device, s_partitionTableDefinitions)) {
            s_partitions += part.get();
        }
    }

    void DetachStorageDevice(NonnullRefPtr<StorageDevice> device) {}
}