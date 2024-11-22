#include <quark/fs/vfs.h>

namespace EXT2 {
    using namespace Quark::System::Io::FileSystem;

    class Ext2Fs : public FileSystem
    {
    public:
        Ext2Fs();
        ~Ext2Fs() = default;
    };
}