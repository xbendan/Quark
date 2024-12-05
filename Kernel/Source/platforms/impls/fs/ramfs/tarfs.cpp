#include <drivers/fs/ramfs/tar_fs.h>

namespace Tar {
    TarFs::TarFs(u64 address, u64 size)
        : FileSystem("tar")
        , m_blocks(reinterpret_cast<Tar::FileInfo*>(address), size / 512)
    {
    }
}