#include <quark/dev/disk_device.h>

namespace Quark::System::Io {
    DiskDevice::DiskDevice(Qk::StringView name)
        : Device(name, Type::DiskDrive)
        , m_partitions{}
    {
    }
}