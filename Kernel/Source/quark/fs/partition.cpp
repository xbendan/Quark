#include <quark/fs/partition.h>

namespace Quark::System::Io::FileSystem {
    Optional<Partition> PartitionTable::GetPartitionByIndex(u64 index) const
    {
        if (index >= m_partitions.len()) {
            return Empty{};
        }

        return m_partitions[index];
    }

    Optional<Partition> PartitionTable::GetPartitionByUuid(
        UUID const& uuid) const
    {
        // for (Partition const& partition : m_partitions) {
        //     if (partition.Get() == uuid) {
        //         return partition;
        //     }
        // }

        return Empty{};
    }

    usize PartitionTable::Count() const
    {
        return m_partitions.len();
    }
}