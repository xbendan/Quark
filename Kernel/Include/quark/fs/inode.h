#include <mixins/std/c++types.h>
#include <mixins/str/string_view.h>

namespace Quark::System::Io::FileSystem {
    using Qk::StringView;

    enum class InodeType : u16
    {
        UNKNOWN,
        FILE,
        DIRECTORY,
        SYMBOLIC_LINK,
        HARD_LINK,
        MOUNT_POINT,
    };
}