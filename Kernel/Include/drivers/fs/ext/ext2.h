#include <mixins/std/c++types.h>
#include <mixins/utils/uuid.h>

namespace EXT2 {
    struct ReserveWindow
    {
        u64 _start;
        u64 _end;
    };

    struct Superblock
    {
        le32 _inodesCount;
        le32 _blocksCount;
        le32 _blocksReservedCount;
        le32 _blocksFreeCount;
        le32 _inodesFreeCount;
        le32 _firstDataBlock;
        le32 _blockSize;
        le32 _fragSize;
        le32 _blocksPerGroup;
        le32 _fragsPerGroup;
        le32 _inodesPerGroup;
        le32 _timeOfMount;
        le32 _timeOfLastWrite;
        le16 _mountCount;
        le16 _maxMountCount;
        le16 _magic;
        le16 _state;
        le16 _errorAction;
        le16 _minorVersion;
        le32 _timeOfLastCheck;
        le32 _checkInterval;
        le32 _osId;
        le32 _majorVersion;
        le16 _uidReserved;
        le16 _gidReserved;

        le32 _firstInode;
        le16 _inodeSize;
        le16 _blockGroupNumber;
        le32 _featureCompat;
        le32 _featureIncompat;
        le32 _featureRoCompat;
        UUID _uuid;
        char _volumeName[16];
        char _lastMounted[64];
        le32 _algoBitmapUsage;

        u8  _preallocBlocks;
        u8  _preallocDirBlocks;
        u16 _padding1;

        UUID _journalUuid;
        u32  _journalInode;
        u32  _journalDevice;
        u32  _orphanInodeListHead;
        u32  _hashSeed[4];
        u8   _defHashVersion;
        u8   _padding2[3];
        le32 _defaultMountOptions;
        le32 _firstMetaBlockGroup;
        le32 _reserved[190];
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