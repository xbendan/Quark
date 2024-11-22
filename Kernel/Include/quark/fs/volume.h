#include <mixins/std/c++types.h>

namespace Quark::System::Io::FileSystem {
    class FileSystem;

    class FsVolume
    {
    public:
    private:
        u8          _id;
        FileSystem* _fileSystem;
    };
} // namespace Quark::System::Io::FileSystem