#include <drivers/partition/gpt/gpt_parser.h>
#include <drivers/partition/gpt/gpt_partition_table.h>
#include <mixins/meta/buf.h>
#include <mixins/utils/checksum.h>
#include <quark/dev/storage/storage_duplexable.h>
#include <quark/os/diagnostic/logging.h>

namespace GPT {
    using namespace Quark::System::Diagnostic;
    using Qk::Array;
    using Qk::Buf;
    using Quark::System::Io::SectorBytes;
    using Quark::System::Io::FileSystem::Partition;

    static const char* GUID_PARTITION_SIGNATURE = "EFI PART";
    static u8          BUFFER[512];

    PartitionTableData::PartitionTableData(Bytes const&                 data,
                                           NonnullRefPtr<StorageDevice> device)
        : m_tableHeader(new GPT::PartitionTableHeader())
    {
        memcpy((void*)&m_tableHeader,
               data.buf(),
               sizeof(GPT::PartitionTableHeader));
        new (&m_partitions) Array<Partition>(m_tableHeader->_count);

        SectorBytes entries(m_tableHeader->_count * sizeof(GPT::PartitionData));
        auto        result = device->Read(m_tableHeader->_lbaEntries, entries);

        if (result.isError() || result.unwrap() < entries.len()) {
            error("[Partition] Failed to read partition entries");
            return;
        }

        for (usize i = 0; i < m_tableHeader->_count; i++) {
            auto* entry = (GPT::PartitionData*)entries.buf() + i;

            auto* partition = new (&m_partitions[i])
                Partition(entry->_lbaBegin, entry->_lbaEnd);
            info$("[Partition] Found GPT partition {}: {} at LBA {} - {}",
                  i,
                  Qk::StringView(entry->_name, strnlen(entry->_name, 72)),
                  partition->GetLBABegin(),
                  partition->GetLBAEnd());
        }
    }

    Optional<PartitionTable*> Parse(NonnullRefPtr<StorageDevice> device)
    {
        Bytes data(BUFFER);
        auto  result = device->Read(0, data);
        if (result.isError() || result.unwrap() < sizeof(BUFFER)) {
            return Empty{};
        }

        auto* table = reinterpret_cast<GPT::PartitionTableHeader*>(BUFFER);
        if (memcmp(&table->_signature, GUID_PARTITION_SIGNATURE, 8) != 0) {
            return Empty{};
        }

        if (!Checksum::VerifyCRC32((u8 const*)table, table->_headerSize)) {
            return Empty{};
        }

        info$("[Partition] Found GPT partition table on device {}\n"
              "   - Revision: {}\n"
              "   - LBA Begin: {}\n"
              "   - LBA End: {}\n"
              "   - Partition Count: {}\n",
              device->GetName(),
              table->_revision,
              table->_lbaBegin,
              table->_lbaEnd);

        return new PartitionTableData(data, device);
    }
}