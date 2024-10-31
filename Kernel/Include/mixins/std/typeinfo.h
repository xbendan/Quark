#include <mixins/str/string.h>
#include <mixins/utils/strings.h>

namespace Std {

    class Typeinfo
    {};
}

namespace Type {
    template <typename T>
    static constexpr StringView GetName()
    {
        StringView name = __PRETTY_FUNCTION__;
        return Strings::Substring(name, Strings::IndexOf(name, "T = ") + 4);
    }
}