#include <quark/dev/disk_device.h>

namespace Quark::System::Io {
    DiskDevice::DiskDevice(string name)
        : Device(name, Type::DiskDrive)
        , m_partitions{}
    {
    }
}