#include <quark/fs/vfs.h>

namespace Quark::System::Io::FileSystem {
    VirtualFs::VirtualFs()
        : FileSystem("Virtual File System", FileSystemInfo())
    {
    }

    Res<NonnullRefPtr<FileAccess>> VirtualFs::Open(StringView   path,
                                                   FileOpenMode mode)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Close(FileAccess* file)
    {
        return Error::NotImplemented();
    }

    Res<NonnullRefPtr<File>> VirtualFs::CreateFile(StringView path)
    {
        return Error::NotImplemented();
    }

    Res<NonnullRefPtr<Directory>> VirtualFs::CreateDirectory(StringView path,
                                                             bool recursively)
    {
        return Error::NotImplemented();
    }

    Res<NonnullRefPtr<Symlink>> VirtualFs::CreateSymlink(StringView path,
                                                         StringView target)
    {
        return Error::NotImplemented();
    }

    Res<NonnullRefPtr<Library>> VirtualFs::CreateLibrary(StringView path)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Unlink(StringView path)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Delete(StringView path)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Rename(StringView oldPath, StringView newPath)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Copy(StringView src,
                          StringView dest,
                          bool       replaceAtDestination)
    {
        return Error::NotImplemented();
    }

    Res<> VirtualFs::Move(StringView src,
                          StringView dest,
                          bool       replaceAtDestination)
    {
        return Error::NotImplemented();
    }

    Res<Array<FileNode*>> VirtualFs::ListFiles(StringView path,
                                               bool       showHiddenFiles,
                                               bool       showSystemFiles)
    {
        return Error::NotImplemented();
    }
}