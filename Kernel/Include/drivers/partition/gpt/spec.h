#define GPT_SIGNATURE 0x5452415020494645

#include <mixins/utils/uuid.h>

namespace GPT {
    struct PartitionEntry
    {
        UUID _partitionId;
        UUID _uniqueId;
        u64  _lbaBegin;
        u64  _lbaEnd;
        u64  _flags;
        char _name[72];
    } __attribute__((packed));

    struct PartitionTableHeader
    {
        u64 _signature;
        u32 _revision;
        u32 _headerSize;
        u32 _checksum;
        u32 __reserved__0;
        u64 _lbaTable;
        u64 _lbaBackup;

        u64 _lbaBegin;
        u64 _lbaEnd;

        UUID _diskIdentifier;

        u64 _lbaEntries;
        u32 _count;
        u32 _entrySize;
        u32 _checksumEntries;
    };
}