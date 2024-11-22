#pragma once

#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/utils/array.h>
#include <mixins/utils/flags.h>
#include <mixins/utils/list.h>
#include <mixins/utils/rbtree.h>
#include <quark/fs/file.h>
#include <quark/fs/file_access.h>
#include <quark/fs/volume.h>

namespace Quark::System::Io::FileSystem {
    using Qk::Array;
    using Qk::List;
    using Qk::NonnullRefPtr;
    using Qk::RbTree;
    using Qk::StringView;

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
        char Name[8];
        u64  MaxSystemSize;
        u64  MaxFileSize;
        u64  MaxFileNameLength;
        u64  MaxPathLength;
        u64  MaxVolumeNameLength;
        u64  MaxVolumePathLength;

        // StringView             FileNameExclusion;
        char8_t                    FileNameExclusion[255];
        Flags<FileSystemAttribute> Attributes;

        constexpr FileSystemInfo(u64                        maxSystemSize,
                                 u64                        maxFileSize,
                                 u64                        maxFileNameLength,
                                 u64                        maxPathLength,
                                 u64                        maxVolumeNameLength,
                                 u64                        maxVolumePathLength,
                                 //  StringView             excludes,
                                 //  char8_t (&fileNameExclusion)[255],
                                 Flags<FileSystemAttribute> attributes)
            : MaxSystemSize(maxSystemSize)
            , MaxFileSize(maxFileSize)
            , MaxFileNameLength(maxFileNameLength)
            , MaxPathLength(maxPathLength)
            , MaxVolumeNameLength(maxVolumeNameLength)
            , MaxVolumePathLength(maxVolumePathLength)
            // , FileNameExclusion(excludes)
            , Attributes(attributes) {};

    protected:
        friend class VirtualFs;
        FileSystemInfo() = default;
    };

    class FileSystem
    {
    public:
        FileSystem(StringView name, FileSystemInfo info)
            : m_name(name)
            , m_info(info) {};

        /**
         * @brief Open a file for reading or writing
         *
         * @param path
         * @param mode
         * @exception FileNotExistException if the file or parent directory do
         * not exist
         * @exception IllegalAccessException if the file cannot be accessed,
         * usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @return @code {.c++}
         * NonnullRefPtr<FileAccess>
         * @endcode
         *  NonnullRefPtr<FileAccess> a non-null reference to the file
         * access record
         */
        virtual Res<NonnullRefPtr<FileAccess>> Open(StringView   path,
                                                    FileOpenMode mode) = 0;

        /**
         * @brief close the file with the specified access record
         *
         * @exception FileNotExistException if the file or parent directory do
         * not exist
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @exception IllegalStateException if the file is not opened or the
         * file access record is invalid
         * @param file the access record of the file
         * @return Res<>
         */
        virtual Res<> Close(FileAccess* file) = 0;

        /**
         * @brief Create a file in the specificed path
         *
         * @exception TargetAlreadyExistException if the file already exists
         * @exception IllegalAccessException if the parent directory cannot be
         * accessed, usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param path
         * @return Res<NonnullRefPtr<File>>
         */
        virtual Res<NonnullRefPtr<File>> CreateFile(StringView path) = 0;

        /**
         * @brief Create a directory in the specified path
         *
         * @exception TargetAlreadyExistException if the directory already
         * exists
         * @exception IllegalAccessException if the parent directory cannot be
         * accessed, usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param path
         * @param recursively whether to create the parent directories
         * @return Res<NonnullRefPtr<Directory>>
         */
        virtual Res<NonnullRefPtr<Directory>> CreateDirectory(
            StringView path,
            bool       recursively) = 0;

        /**
         * @brief Create a Symlink object
         *
         * @exception TargetAlreadyExistException if the symlink already exists
         * @exception IllegalAccessException if the parent directory cannot be
         * accessed, usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param path
         * @param target
         * @return Res<NonnullRefPtr<Symlink>>
         */
        virtual Res<NonnullRefPtr<Symlink>> CreateSymlink(
            StringView path,
            StringView target) = 0;

        /**
         * @brief Create a library, aka the shared directory set
         *
         * @param path
         * @return Res<NonnullRefPtr<Library>>
         */
        virtual Res<NonnullRefPtr<Library>> CreateLibrary(StringView path) = 0;

        /**
         * @brief Reduce the reference count of the file by 1
         *
         * Unlinking a file will not delete the file, but only reduce the
         * reference count by 1. If the reference count reaches 0, the file
         * will be deleted.
         *
         * @exception FileNotExistException if the file or parent directory do
         * not exist
         * @exception IllegalAccessException if the file cannot be accessed,
         * usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param path
         * @return Res<>
         */
        virtual Res<> Unlink(StringView path) = 0;

        /**
         * @brief Delete the file or directory
         *
         * Delete is a force operation, which will delete the file or directory
         * directly without any confirmation. If the file is opened, it will be
         * closed first, unless the file is locked or read into memory.
         *
         * @exception FileNotExistException if the file or parent directory do
         * not exist
         * @exception IllegalAccessException if the file cannot be accessed,
         * usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param path
         * @return Res<>
         */
        virtual Res<> Delete(StringView path) = 0;

        virtual Res<> Rename(StringView oldPath, StringView newPath) = 0;

        /**
         * @brief Create a copy of the file at destination
         *
         * @exception FileNotExistException if the source or its parent
         * directory do not exist
         * @exception TargetAlreadyExistException if the destination already
         * exists
         * @exception IllegalAccessException if the source or destination cannot
         * be accessed, usually due to permission issues
         * @exception IllegalArgumentException if the path is null, invalid,
         * differ from the expected format, containes invalid characters, or is
         * too long, etc.
         * @param src
         * @param dest
         * @return Res<>
         */
        virtual Res<> Copy(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) = 0;

        /**
         * @brief Move the file from source to destination
         *
         * @exception FileNotExistException if the source or its parent
         * directory do not exist
         * @param src
         * @param dest
         * @return Res<>
         */
        virtual Res<> Move(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) = 0;

        /**
         * @brief List all files in the specified directory
         *
         * @param path
         * @param showHiddenFiles
         * @param showSystemFiles
         * @return Res<Array<FileNode*>>
         */
        virtual Res<Array<FileNode*>> ListFiles(
            StringView path,
            bool       showHiddenFiles = false,
            bool       showSystemFiles = false) = 0;

        FileSystemInfo const& GetSystemInfo() { return m_info; }

    private:
        StringView     m_name;
        FileSystemInfo m_info;
    };

    class VirtualFs : public FileSystem
    {
    public:
        VirtualFs();
        ~VirtualFs() = default;

        virtual Res<NonnullRefPtr<FileAccess>> Open(StringView   path,
                                                    FileOpenMode mode) override;

        virtual Res<> Close(FileAccess* file) override;

        virtual Res<NonnullRefPtr<File>> CreateFile(StringView path) override;

        virtual Res<NonnullRefPtr<Directory>> CreateDirectory(
            StringView path,
            bool       recursively) override;

        virtual Res<NonnullRefPtr<Symlink>> CreateSymlink(
            StringView path,
            StringView target) override;

        virtual Res<NonnullRefPtr<Library>> CreateLibrary(
            StringView path) override;

        virtual Res<> Unlink(StringView path) override;

        virtual Res<> Delete(StringView path) override;

        virtual Res<> Rename(StringView oldPath, StringView newPath) override;

        virtual Res<> Copy(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) override;

        virtual Res<> Move(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) override;

        virtual Res<Array<FileNode*>> ListFiles(
            StringView path,
            bool       showHiddenFiles = false,
            bool       showSystemFiles = false) override;

    private:
        FileNode*                     m_root;
        List<FsVolume*>               m_volumes;
        RbTree<StringView, FileNode*> m_mappings;
    };
} // namespace Quark::System::Io::FileSystem