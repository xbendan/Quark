#include <mixins/std/string.h>

namespace Std {

    class Typeinfo
    {};
}

namespace Type {
    template <typename T>
    static constexpr string GetName()
    {
        string name = __PRETTY_FUNCTION__;
        return name.Substring(name.IndexOf("T = ") + 4);
    }
}