#include <quark/dev/storage/storage_device.h>
#include <quark/dev/storage/storage_device_partition.h>

namespace Quark::System::Io {
    Res<usize> StorageDevicePartition::Read(u64 offset, Bytes buffer)
    {
        return m_device->Read(m_partition.GetLBABegin(), buffer);
    }

    Res<usize> StorageDevicePartition::Write(u64 offset, Bytes buffer)
    {
        return m_device->Write(m_partition.GetLBABegin(), buffer);
    }
}