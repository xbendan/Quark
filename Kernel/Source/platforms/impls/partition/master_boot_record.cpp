#include <drivers/partition/mbr/mbr_parser.h>
#include <drivers/partition/mbr/partition_table.h>
#include <drivers/partition/mbr/spec.h>
#include <quark/os/diagnostic/logging.h>

namespace MBR {
    using namespace Quark::System::Diagnostic;

    static byte DiskBuffer[512];

    MasterBootRecord::MasterBootRecord(Bytes const& data, StorageDevice* device)
        : PartitionTable()
        , m_lbaBegin(0)
    {
        // usize bytesRead;
        // if ((bytesRead = result.unwrap()) != 512) {
        //     error$("Failed to read Master Boot Record from device {}, where "
        //            "only {} bytes were read",
        //            device->GetName(),
        //            bytesRead);
        //     return;
        // }

        // info$("Master Boot Record successfully read from device {}",
        //       device->GetName());

        m_partitions = Array<Partition>(4);
        auto* table =
            reinterpret_cast<MBR::PartitionTableData const*>(data.buf());
        for (usize i = 0; i < 4; ++i) {
            auto const& entry = table->_entries[i];
            if (entry._lbaBegin == 0 || entry._type == 0) {
                continue;
            }

            info$("Partition {} - Type: {}, Start LBA: {}, Size: {}",
                  i,
                  entry._type,
                  entry._lbaBegin,
                  entry._lbaLength);
            new (&m_partitions[i]) Partition(
                entry._lbaBegin, entry._lbaBegin + entry._lbaLength - 1);
        }
    }

    Optional<PartitionTable*> EnumeratePartitions(
        NonnullRefPtr<StorageDevice> device)
    {
        Bytes data(DiskBuffer);
        auto  result = device->Read(0, data);
        if (result.isError() || result.unwrap() < sizeof(DiskBuffer)) {
            return Empty{};
        }

        auto* table = reinterpret_cast<MBR::PartitionTableData*>(DiskBuffer);
        if (table->_signature != MBR::SIGNATURE) {
            return Empty{};
        }

        // No checksum verification for MBR

        info$("[Partition] Found MBR partition table on device {}\n"
              "   - Disk ID: {}\n",
              device->GetName(),
              table->_diskId);

        return new MasterBootRecord(data, device.get());
    }
}