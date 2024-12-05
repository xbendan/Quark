#include <mixins/std/c++types.h>

namespace Quark::System::Io::FileSystem {
    class FileSystem;

    class FsVolume
    {
    public:
    private:
        FileSystem* _fileSystem;
    };
} // namespace Quark::System::Io::FileSystem