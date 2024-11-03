#include <mixins/str/string_view.h>

namespace Qk {
    struct Breakpoint
    {
        StringView File{};
        StringView Function{};
        usize      Line{};
        usize      Column{};

        static constexpr Breakpoint current(
            StringView file     = __builtin_FILE(),
            StringView function = __builtin_FUNCTION(),
            usize      line     = __builtin_LINE(),
            usize      column   = __builtin_COLUMN())
        {
            return { file, function, line, column };
        }
    };
}