#include <drivers/partition/gpt/spec.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>

namespace GPT {
    using Quark::System::Io::FileSystem::PartitionTable;

    class PartitionTableData : public PartitionTable
    {
    public:
        PartitionTableData(Bytes const& data, NonnullRefPtr<StorageDevice>);

    private:
        GPT::PartitionTableHeader* m_tableHeader;
    };
}