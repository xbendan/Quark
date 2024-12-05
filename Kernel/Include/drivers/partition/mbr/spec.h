#include <mixins/std/c++types.h>

namespace MBR /* Master Boot Record */ {
    struct PartitionEntry
    {
        u8  _status;
        u8  _chsBegin[3];
        u8  _type;
        u8  _chsEnd[3];
        u32 _lbaBegin;
        u32 _lbaLength;
    } __attribute__((packed));

    struct PartitionTableData
    {
        u8             _bootCode[440];
        u32            _diskId;
        u16            __reserved__0;
        PartitionEntry _entries[4];
        u16            _signature;
    } __attribute__((packed));

    static const u16 SIGNATURE = 0xAA55;
}