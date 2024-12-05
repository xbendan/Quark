#include <drivers/partition/gpt/gpt_parser.h>
#include <drivers/partition/mbr/mbr_parser.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/dev/storage/storage_device_partition.h>

namespace Quark::System::Io {
    Res<> StorageDevice::OnInitialize()
    {
        return Ok();
    }
}