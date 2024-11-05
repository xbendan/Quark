#include <mixins/utils/list.h>
#include <quark/dev/device.h>
#include <quark/fs/partition.h>

namespace Quark::System::Io {
    using Qk::List;

    class DiskDevice : public Device
    {
    public:
        DiskDevice(Qk::StringView name);
        virtual ~DiskDevice() = default;

        virtual i64 Read(u64 offset, u64 size, void* buffer)  = 0;
        virtual i64 Write(u64 offset, u64 size, void* buffer) = 0;

    private:
        List<FileSystem::Partition> m_partitions;
    };
} // namespace Quark::System::Io