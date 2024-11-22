#include <quark/fs/partition.h>

namespace GPT {
    using Quark::System::Io::FileSystem::PartitionTable;

    class GuidPartitionTable : public PartitionTable
    {
    public:
        GuidPartitionTable();
    };
}