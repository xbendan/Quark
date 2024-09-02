#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/array.h>
#include <mixins/utils/flags.h>
#include <mixins/utils/uuid.h>

namespace Quark::Io::FileSystem {
    enum class FileType : u16
    {
        Unknown,
        File,
        Directory,
        Device,
        Pipe,
        Socket,
        SymbolicLink,
        HardLink,
        MountPoint,
    };

    enum class FileMode
    {
        Append,
        Create,
        CreateIfAbsent,
        Open,
        OpenOrCreate,
    };

    enum class FileAttribute : u64
    {
        ReadOnly = 1,
        Hidden,
        Archive,
        Compressed,
        Encrypted,
        Temporary
    };
    MakeFlags$(FileAttribute);

    enum class FileSource
    {
        Local,
        Network,
        Cloud,
        Virtual,
        Device
    };

    class INode
    {
    public:
        virtual i64  read(u64 offset, u64 size, u8* buffer)  = 0;
        virtual i64  write(u64 offset, u64 size, u8* buffer) = 0;
        virtual void close()                                 = 0;
    };

    class FileNode
    {
    public:
        FileNode(FileSource source,
                 FileType   type,
                 String<>   name,
                 String<>   parent)
            : m_source(source)
            , m_type(type)
            , m_name(name)
            , m_parent(parent)
        {
        }

        bool isRemote() const { return m_source != FileSource::Local; }

    protected:
        FileSource m_source;
        FileType   m_type;
        String<>   m_name;
        String<>   m_parent;
    };

    class Folder : public FileNode
    {
    public:
        Folder() = default;
        Folder(String<> fullpath);
        virtual ~Folder();

        virtual Res<Array<FileNode*>> listFiles() = 0;
    };

    class File : public FileNode
    {
    public:
        File() = default;
        File(String<> fullpath);
        virtual ~File();

        virtual Res<i64> read(u64 offset, u64 size, u8* buffer)  = 0;
        virtual Res<i64> write(u64 offset, u64 size, u8* buffer) = 0;
        virtual void     close()                                 = 0;

    protected:
        UUID m_uniqueId;
        u64  m_size;
        u64  m_bytesTaken;

        Flags<FileAttribute> m_attributes;
    };

    class IFileSystem;

    class IFileAccess
    {
    public:
        IFileAccess(File* file, IFileSystem& fileSystem, FileMode mode)
            : m_file(file)
            , m_fileSystem(fileSystem)
            , m_accessMode(mode)
        {
        }

        virtual i64  read(u64 offset, u64 size, u8* buffer)  = 0;
        virtual i64  write(u64 offset, u64 size, u8* buffer) = 0;
        virtual void close()                                 = 0;

    private:
        File*        m_file;
        IFileSystem& m_fileSystem;
        FileMode     m_accessMode;
    };
}