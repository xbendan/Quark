#pragma once

#include <mixins/meta/result.h>
#include <mixins/utils/flags.h>
#include <quark/fs/file.h>

namespace Quark::System::Io::FileSystem {
    enum class FileSystemId
    {
        Virtual,
        Ext2,
        Ext3,
        Ext4,
        FAT32,
        NTFS,
        HFS,
        HFSPlus,
        ZFS,
    };

    enum class FileSystemAttribute
    {
        CaseSensitive,
        Extensible,
        Journaling,
        Encryption,
        Compression,
        Deduplication,
        Snapshots,
    };

    struct FileSystemInfo
    {
    public:
        u64 _maxSystemSize;
        u64 _maxFileSize;
        u64 _maxFileNameLength;
        u64 _maxPathLength;
        u64 _maxVolumeNameLength;
        u64 _maxVolumePathLength;

        String<>                   _excludes;
        Flags<FileSystemAttribute> _attributes;

        FileSystemInfo(u64                        maxSystemSize,
                       u64                        maxFileSize,
                       u64                        maxFileNameLength,
                       u64                        maxPathLength,
                       u64                        maxVolumeNameLength,
                       u64                        maxVolumePathLength,
                       String<>                   excludes,
                       Flags<FileSystemAttribute> attributes)
            : _maxSystemSize(maxSystemSize)
            , _maxFileSize(maxFileSize)
            , _maxFileNameLength(maxFileNameLength)
            , _maxPathLength(maxPathLength)
            , _maxVolumeNameLength(maxVolumeNameLength)
            , _maxVolumePathLength(maxVolumePathLength)
            , _excludes(excludes)
            , _attributes(attributes) {};

    protected:
        friend class VirtualFileSystem;
        FileSystemInfo() = default;
    };

    class IFileSystem
    {
    public:
        IFileSystem(String<> name, FileSystemId id, FileSystemInfo info)
            : m_name(name)
            , m_id(id)
            , m_info(info) {};

        virtual Res<IFileAccess*> open(String<> path, FileMode mode)   = 0;
        virtual Res<IFileAccess*> open(File* file, FileMode mode)      = 0;
        virtual Res<>             close(File* file)                    = 0;
        virtual Res<>             create(String<> path, FileType type) = 0;
        virtual Res<>             remove(String<> path)                = 0;

        FileSystemId    getId() const { return m_id; }
        FileSystemInfo& getInfo() { return m_info; }

    private:
        String<>       m_name;
        FileSystemId   m_id;
        FileSystemInfo m_info;
    };

    class VirtualFileSystem : public IFileSystem
    {
    public:
        VirtualFileSystem()
            : IFileSystem("Virtual File System",
                          FileSystemId::Virtual,
                          FileSystemInfo())
        {
        }
    };
} // namespace Quark::System::Io::FileSystem