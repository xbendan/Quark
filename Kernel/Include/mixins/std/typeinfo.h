#include <mixins/std/string.h>

namespace Std {

    class Typeinfo
    {};
}

namespace Type {
    template <typename T>
    static constexpr string GetName()
    {
        return string(__PRETTY_FUNCTION__)
            .Substring(__PRETTY_FUNCTION__.IndexOf("T = ") + 4);
    }
}