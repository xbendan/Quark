#include <drivers/fs/ramfs/tar_spec.h>
#include <mixins/meta/ref.h>
#include <mixins/str/string_view.h>
#include <mixins/utils/list.h>
#include <quark/fs/file_access.h>
#include <quark/fs/vfs.h>

namespace Tar {
    using Qk::List;
    using Qk::NonnullRefPtr;
    using Qk::StringView;
    using namespace Quark::System::Io::FileSystem;

    constexpr inline usize oct2bin(byte* c, usize size)
    {
        usize n = 0;
        while (size-- && *c) {
            n <<= 3;
            n |= (*c - '0') & 0x7;
            c++;
        }
        return n;
    }

    class TarFs : public FileSystem
    {
    public:
        TarFs(u64 address, u64 size);

        virtual Res<NonnullRefPtr<FileAccess>> Open(
            StringView   path,
            FileOpenMode mode) override final;

        virtual Res<> Close(FileAccess* file) override final;

        virtual Res<NonnullRefPtr<File>> CreateFile(
            StringView path) override final;

        virtual Res<NonnullRefPtr<Directory>> CreateDirectory(
            StringView path,
            bool       recursively) override final;

        virtual Res<NonnullRefPtr<Symlink>> CreateSymlink(
            StringView path,
            StringView target) override final;

        virtual Res<NonnullRefPtr<Library>> CreateLibrary(
            StringView path) override final;

        virtual Res<> Unlink(StringView path) override final;

        virtual Res<> Delete(StringView path) override final;

        virtual Res<> Rename(StringView oldPath,
                             StringView newPath) override final;

        virtual Res<> Copy(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) override final;

        virtual Res<> Move(StringView src,
                           StringView dest,
                           bool       replaceAtDestination) override final;

        virtual Res<Array<FileNode*>> ListFiles(
            StringView path,
            bool       showHiddenFiles = false,
            bool       showSystemFiles = false) override final;

    private:
        Array<Tar::FileInfo> m_blocks;
        List<Tar::FileInfo*> m_files;
    };
}