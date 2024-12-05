#include <drivers/partition/gpt/gpt_definition.h>
#include <drivers/partition/gpt/gpt_partition_table.h>
#include <mixins/meta/buf.h>
#include <mixins/utils/checksum.h>
#include <quark/dev/storage/storage_device.h>
#include <quark/dev/storage/storage_duplexable.h>
#include <quark/os/diagnostic/logging.h>

namespace GPT {
    using namespace Quark::System::Diagnostic;
    using Qk::Array;
    using Qk::Buf;
    using Quark::System::Io::SectorBytes;
    using Quark::System::Io::FileSystem::Partition;

    static const char* GUID_PARTITION_SIGNATURE = "EFI PART";

    PartitionTableData::PartitionTableData(
        Bytes const&                     data,
        NonnullRefPtr<Io::StorageDevice> device)
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

    Res<NonnullRefPtr<Io::FileSystem::PartitionTable>>
    PartitionTableDefinition::Read(NonnullRefPtr<Io::StorageDevice> device)
    {
        SectorBytes data(512);
        if (auto result = device->Read(0, data); result.isError()) {
            return Error::InvalidOperation("Failed to read partition table");
        }

        auto* tableData =
            reinterpret_cast<GPT::PartitionTableHeader const*>(data.buf());
        if (memcmp(&tableData->_signature, GUID_PARTITION_SIGNATURE, 8) != 0) {
            return Error::InvalidArgument("Invalid GPT signature");
        }

        if (!Checksum::VerifyCRC32((u8 const*)tableData,
                                   tableData->_headerSize)) {
            return Error::InvalidArgument("Invalid GPT header checksum");
        }

        info$("[Partition] Found GPT partition table on device {}\n"
              "   - Revision: {}\n"
              "   - LBA Begin: {}\n"
              "   - LBA End: {}\n"
              "   - Partition Count: {}\n",
              device->GetName(),
              tableData->_revision,
              tableData->_lbaBegin,
              tableData->_lbaEnd);

        return Ok(NonnullRefPtr<PartitionTable>{
            new GPT::PartitionTableData(data, device) });
    }

    Res<> PartitionTableDefinition::Write(
        NonnullRefPtr<Io::StorageDevice>,
        NonnullRefPtr<Io::FileSystem::PartitionTable>)
    {
        return Ok();
    }

    Res<> PartitionTableDefinition::Erase(NonnullRefPtr<Io::StorageDevice>)
    {
        return Ok();
    }

    Res<> PartitionTableDefinition::Format(NonnullRefPtr<Io::StorageDevice>)
    {
        return Ok();
    }
}