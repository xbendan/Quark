#define GPT_SIGNATURE 0x5452415020494645

#include <mixins/utils/uuid.h>

namespace GPT {
    struct ProtectiveMasterBootRecord
    {
        u8  _bootCode{ 0x00 };
        u8  _chsBegin[3]{ 0x00, 0x02, 0x00 };
        u8  _type{ 0xEE /* GPT Protective */ };
        u8  _chsEnd[3]{ 0xFF, 0xFF, 0xFF };
        u32 _lbaBegin{ 1 };
        u32 _lbaLength{ 0xFFFFFFFF };
    } __attribute__((packed));

    struct PartitionData
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
        u8  __reserved__1[512 - 0x5C];
    };
}