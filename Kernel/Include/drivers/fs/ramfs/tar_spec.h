#include <mixins/std/c++types.h>

#define TAR_FILE_FILENAME_LENGTH 100

namespace Tar {
    enum class FileType : byte
    {
        NORMAL_FILE      = '0', // or NUL character, aka 0x00
        HARD_LINK        = '1',
        SYMBOLIC_LINK    = '2',
        CHARACTER_DEVICE = '3',
        BLOCK_DEVICE     = '4',
        DIRECTORY        = '5',
        NAMED_PIPILINE   = '6',
    };

    struct FileInfo
    {
        char     Name[TAR_FILE_FILENAME_LENGTH];
        u64      Mode;
        u64      OwnerId;
        u64      GroupId;
        byte     Size[12];
        byte     LastModified[12];
        byte     Checksum[8];
        FileType Type;
        char     LinkName[TAR_FILE_FILENAME_LENGTH];
        char     UStarIndicator[6];
        char     UStarVersion[2]{ '0', '0' };
        char     OwnerName[32];
        char     GroupName[32];
        u64      DeviceMajor;
        u64      DeviceMinor;
        char     Prefix[155];
    };
}