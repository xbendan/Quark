#include <mixins/str/string.h>
#include <mixins/str/string_view.h>

namespace Strings {
    using Unit = String::Unit;

    String Concat(Sliceable<Unit> auto& begin, Sliceable<Unit> auto& end);

    String Concat(Sliceable<Unit> auto& begin, const char* end);

    String Concat(const char* begin, Sliceable<Unit> auto& end);

    bool Contains(Sliceable<Unit> auto& string, Unit code);

    StringView Substring(StringView string, usize begin, usize end);

    StringView Substring(StringView string, usize begin);

    StringView Substring(StringView string, Sliceable<Unit> auto& begin);

    String Substring(String const& string, usize begin, usize end);

    String Substring(String const& string, usize begin);

    String Substring(String const& string, Sliceable<Unit> auto& begin);

    bool StartsWith(Sliceable<Unit> auto& string, Sliceable<Unit> auto& prefix);

    bool StartsWith(Sliceable<Unit> auto& string, const char* prefix);

    bool StartsWith(Sliceable<Unit> auto& string, Unit prefix);

    bool EndsWith(Sliceable<Unit> auto& string, Sliceable<Unit> auto& suffix);

    bool EndsWith(Sliceable<Unit> auto& string, const char* suffix);

    bool EndsWith(Sliceable<Unit> auto& string, Unit suffix);

    bool Equals(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs);

    bool ContentEquals(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs);

    int IndexOf(Sliceable<Unit> auto& string, Unit code);

    int IndexOf(Sliceable<Unit> auto& string, Sliceable<Unit> auto& substring);

    int IndexOf(Sliceable<Unit> auto& string, const char* substring);

    int LastIndexOf(Sliceable<Unit> auto& string, Unit code);

    int LastIndexOf(Sliceable<Unit> auto& string,
                    Sliceable<Unit> auto& substring);

    int LastIndexOf(Sliceable<Unit> auto& string, const char* substring);

    String Replace(Sliceable<Unit> auto& string,
                   Sliceable<Unit> auto& target,
                   Sliceable<Unit> auto& replacement);

    String ToUpperCase(Sliceable<Unit> auto& string);

    String ToLowerCase(Sliceable<Unit> auto& string);

    String Trim(Sliceable<Unit> auto& string);

    String TrimStart(Sliceable<Unit> auto& string);

    String TrimEnd(Sliceable<Unit> auto& string);

    usize len(Sliceable<Unit> auto& string);

    usize len(const char* string);

    String operator""_s(const char* str, usize len);

    String operator""_s(Unit const* str, usize len);

    String operator+(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs);

    String operator+(Sliceable<Unit> auto& lhs, const char* rhs);

    String operator+(const char* lhs, Sliceable<Unit> auto& rhs);
}