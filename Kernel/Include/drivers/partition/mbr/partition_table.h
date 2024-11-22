#include <quark/dev/storage/storage_device.h>
#include <quark/fs/partition.h>

namespace MBR {
    using Qk::Array;
    using Quark::System::Io::StorageDevice;
    using Quark::System::Io::FileSystem::Partition;

    class MasterBootRecord
        : public Quark::System::Io::FileSystem::PartitionTable
    {
    public:
        MasterBootRecord(StorageDevice*);
        ~MasterBootRecord() = default;

    private:
        bool      m_valid;
        u32 const m_lbaBegin;
        Bytes     m_cache;
    };
}