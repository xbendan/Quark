#include <drivers/partition/mbr/partition_table.h>
#include <drivers/partition/mbr/spec.h>
#include <quark/os/diagnostic/logging.h>

namespace MBR {
    using namespace Quark::System::Diagnostic;

    MasterBootRecord::MasterBootRecord(StorageDevice* device)
        : PartitionTable()
        , m_valid(false)
        , m_lbaBegin(0)
        , m_cache({ new u8[512], 512 })
    {
        auto result = device->Read(m_lbaBegin, m_cache);

        if (result.isError()) {
            error$("Failed to read Master Boot Record from device {}",
                   device->GetName());
            return;
        }

        usize bytesRead;
        if ((bytesRead = result.unwrap()) != 512) {
            error$("Failed to read Master Boot Record from device {}, where "
                   "only {} bytes were read",
                   device->GetName(),
                   bytesRead);
            return;
        }

        m_valid = true;

        info$("Master Boot Record successfully read from device {}",
              device->GetName());

        m_partitions = Array<Partition>(4);
        auto* table =
            reinterpret_cast<MBR::PartitionTable const*>(m_cache.buf());
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
}