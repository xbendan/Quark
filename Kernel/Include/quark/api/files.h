#include <quark/fs/file.h>

namespace Quark::System::API {
    using namespace Quark::System::Io::FileSystem;

    Res<File*>        getFileInfo(String<> path);
    Res<IFileAccess*> openFile(const char* path, FileMode mode);
}