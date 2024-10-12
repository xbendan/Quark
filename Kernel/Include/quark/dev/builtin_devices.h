#include <mixins/utils/linked_list.h>
#include <quark/dev/device.h>
#include <quark/fs/partition.h>

namespace Quark::System::Io {
    class DiskDevice : public Device
    {
    public:
        DiskDevice(string name);
        virtual ~DiskDevice();

        virtual i64 Read(u64 offset, u64 size, void* buffer)  = 0;
        virtual i64 Write(u64 offset, u64 size, void* buffer) = 0;

    private:
        LinkedList<FileSystem::Partition> m_partitions;
    };
} // namespace Quark::System::Io