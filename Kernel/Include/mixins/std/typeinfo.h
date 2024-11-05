#pragma once

#include <mixins/str/string.h>
#include <mixins/utils/strings.h>

namespace Std {

    class Typeinfo
    {};
}

namespace Type {
    template <typename T>
    static constexpr Qk::StringView GetName()
    {
        Qk::StringView name = __PRETTY_FUNCTION__;
        return Qk::Strings::Substring(name,
                                      Qk::Strings::IndexOf(name, "T = ") + 4);
    }
}