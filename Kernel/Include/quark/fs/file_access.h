#pragma once

#include <quark/fs/file.h>

namespace Quark::System::Io::FileSystem {
    struct FileAccess
    {
        File*        _file;
        FileOpenMode _mode;
        usize        _offset;
        struct State
        {
            bool IsOpen : 1 { false };
            bool IsReadable : 1 { false };
            bool IsWritable : 1 { false };
            bool IsDirectory : 1 { false };
            bool IsBlocked : 1 { true };
        };
    };
}