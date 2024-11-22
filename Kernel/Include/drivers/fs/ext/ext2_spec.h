#include <mixins/std/c++types.h>
#include <mixins/utils/uuid.h>

// Reference: https://github.com/torvalds/linux/blob/master/fs/ext2/ext2.h

namespace EXT2 {
    struct ReserveWindow
    {
        u64 _start;
        u64 _end;
    };

    struct ReserveWindowNode
    {
        u32           GoalSize;
        u32           AllocHits;
        ReserveWindow Window;
    };

    struct BlockAllocInfo
    {
        ReserveWindowNode WindowNode;
        u32               LastAllocLogicBlock;
        u64               LastAllocPhysBlock;
    };

    struct Superblock
    {
        le32 InodesCount;
        le32 BlocksCount;
        le32 BlocksReservedCount;
        le32 BlocksFreeCount;
        le32 InodesFreeCount;
        le32 FirstDataBlock;
        le32 BlockSize;
        le32 FragSize;
        le32 BlocksPerGroup;
        le32 FragsPerGroup;
        le32 InodesPerGroup;
        le32 TimeOfMount;
        le32 TimeOfLastWrite;
        le16 MountCount;
        le16 MaxMountCount;
        le16 Magic;
        le16 State;
        le16 ErrorAction;
        le16 MinorVersion;
        le32 TimeOfLastCheck;
        le32 CheckInterval;
        le32 OsId;
        le32 MajorVersion;
        le16 UidReserved;
        le16 GidReserved;

        le32 FirstInode;
        le16 InodeSize;
        le16 BlockGroupNumber;
        le32 FeatureCompat;
        le32 FeatureIncompat;
        le32 FeatureRoCompat;
        UUID Uuid;
        char VolumeName[16];
        char LastMounted[64];
        le32 AlgoBitmapUsage;

        u8  PreallocBlocks;
        u8  PreallocDirBlocks;
        u16 Padding1;

        UUID JournalUuid;
        u32  JournalInode;
        u32  JournalDevice;
        u32  OrphanInodeListHead;
        u32  HashSeed[4];
        u8   DefHashVersion;
        u8   Padding2[3];
        le32 DefaultMountOptions;
        le32 FirstMetaBlockGroup;
        le32 __reserved__[190];
    };

    struct GroupDesc
    {
        le32 _blockBitmap;
        le32 _inodeBitmap;
        le32 _inodeTable;
        le16 _freeBlocksCount;
        le16 _freeInodesCount;
        le16 _directoriesCount;
        le16 _pad;
        le32 _reserved[3];
    };

    struct Inode
    {
        le16 _mode;
        le16 _bitsUid;
        le32 _size;
        le32 _timeOfAccess;
        le32 _timeOfCreation;
        le32 _timeOfModification;
        le32 _timeOfDeletion;
        le16 _bitsGroupId;
        le16 _linksCount;
        le32 _blocksCount;
        le32 _flags;

        le32 _osd1;

        le32 _blocks[15];
        le32 _fileVersion;
        le32 _fileAcl;
        le32 _dirAcl;
        le32 _fragmentAddress;

        struct
        {
            u8   _fragNum;
            u8   _fragSize;
            u16  _pad;
            le16 _uidHigh;
            le16 _gidHigh;
            u32  _reserved;
        };
    };
} // namespace EXT2