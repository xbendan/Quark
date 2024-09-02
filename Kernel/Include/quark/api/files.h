#include <quark/fs/file.h>

namespace Quark::API {
    using namespace Quark::Io::FileSystem;

    Res<File*>        getFileInfo(String<> path);
    Res<IFileAccess*> openFile(const char* path, FileMode mode);
}