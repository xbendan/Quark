#include <mixins/std/c++types.h>

namespace FAT32 {
    struct BIOSParamBlock {
        u16 _bytesPerSector;
        u8  _sectorsPerCluster;
        u16 _reservedSectors;
        u8  _fatCount;
        u16 _rootEntryCount;
        u16 _totalSectors16;
        u8  _media;
        u16 _sectorsPerFat16;
        u16 _sectorsPerTrack;
        u16 _headCount;
        u32 _hiddenSectors;
        u32 _totalSectors32;
        u32 _sectorsPerFat32;

        union {
            u16 _extFlags;

            struct {
                u16 _activeFat: 4;
                u16 _reserved1: 3;
                u16 _fatMirrored: 1;
                u16 _reserved2: 8;
            };
        };

        u16 _version;
        u32 _rootCluster;
        u16 _fsInfo;
        u16 _backupBootSector;
    };

    struct ExtBootRecord {
        u32 _sectorsPerFat;
        u16 _flags;
        u16 _version;
        u32 _rootCluster;
        u16 _fsInfo;
        u16 _backupBootSector;
        u8  _reserved[12];
        u8  _driveNumber;
        u8  _reserved1;
        u8  _bootSignature;
        u32 _volumeId;
        u8  _volumeLabel[11];
        u8  _fileSystemType[8];
    };

    struct FsInfo {
        u32 _leadSignature;
        u8  _reserved1[480];
        u32 _structSignature;
        u32 _freeClusterCount;
        u32 _nextFreeCluster;
        u8  _reserved2[12];
        u32 _trailSignature;
    };

    struct BootRecord {
        BIOSParamBlock _biosParamBlock;
        ExtBootRecord  _ext;
        FsInfo         _fsInfo;
    };

    struct Entry {
        u8  _filename[8];
        u8  _extension[3];
        u8  _attributes;
        u8  _reserved;
        u8  _creationTimeTenths;
        u16 _creationTime;
        u16 _creationDate;
        u16 _lastAccessDate;
        u16 _firstClusterHigh;
        u16 _lastWriteTime;
        u16 _lastWriteDate;
        u16 _firstClusterLow;
        u32 _fileSize;
    };

    struct LongEntry {
        u8  _ord;
        u16 _name0[5];
        u8  _attributes;
        u8  _type;
        u8  _checksum;
        u16 _name1[6];
        u16 _firstCluster;
        u16 _name2[2];
    };
}  // namespace FAT32