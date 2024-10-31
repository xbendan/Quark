#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/str/string.h>
#include <quark/dev/device.h>

namespace {
    struct OsModule
    {
        StringView _name;
        u64        _key;

        Func<Quark::System::Io::Device*(void)> _init;
    };
}