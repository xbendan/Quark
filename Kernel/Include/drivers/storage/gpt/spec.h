#include <mixins/std/c++types.h>
#include <mixins/utils/uuid.h>

namespace GPT {
    using namespace Mixins::Utils;

    struct GuidPartTable {
        u64  _signature;
        u32  _revision;
        u32  _headerSize;
        u32  _crc32Header;
        u32  __reserved__0;
        u64  _currentLba;
        u64  _backupLba;
        u64  _firstUsableLba;
        u64  _lastUsableLba;
        UUID _guid;
        u64  _partitionEntryLba;
        u32  _numPartitionEntries;
        u32  _partitionEntrySize;
        u32  _crc32PartitionArray;
    } __attribute__((packed));

    struct GuidPartEntry {
        UUID _partitionTypeGuid;
        UUID _uniquePartitionGuid;
        u64  _startingLba;
        u64  _endingLba;
        u64  _attributes;
        u16  _name[36];
    } __attribute__((packed));
}  // namespace GPT