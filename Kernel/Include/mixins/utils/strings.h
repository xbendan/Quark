#pragma once

#include <mixins/str/string.h>
#include <mixins/str/string_builder.h>
#include <mixins/str/string_view.h>
#include <mixins/utils/chars.h>

namespace Qk::Strings {
    using Encoding = Qk::StringView::Encoding;
    using Unit     = Qk::StringView::Unit;

    inline String Concat(Sliceable<Unit> auto& begin, Sliceable<Unit> auto& end)
    {
        StringBuilder<> result(begin.len() + end.len() + 1);
        result.append(begin);
        result.append(end);
        return result.getString();
    }

    inline String Concat(Sliceable<Unit> auto& begin, const char* end)
    {
        return Concat(begin, StringView{ end });
    }

    inline String Concat(const char* begin, Sliceable<Unit> auto& end)
    {
        return Concat(StringView{ begin }, end);
    }

    bool Contains(Sliceable<Unit> auto& str, Unit code)
    {
        for (Unit c : str) {
            if (c == code)
                return true;
        }
        return false;
    }

    bool Contains(Sliceable<Unit> auto& str, Rune rune)
    {
        for (Rune r : iterRunes(str)) {
            if (r == rune)
                return true;
        }
        return false;
    }

    inline StringView Substring(StringView str, usize begin, usize end)
    {
        assert(begin <= end && end <= str.len(),
               Error::IndexOutOfBounds("Invalid range"));

        usize rbegin = 0, rend = 0;
        usize len;
        for (Rune r : iterRunes(str)) {
            len = Encoding::getRuneLength(r);
            if (begin) {
                begin--;
                rbegin += len;
            }

            if (end--) {
                rend += len;
            } else
                break;
        }
        return { str.buf() + rbegin, rend - rbegin };
    }

    inline StringView Substring(StringView str, usize begin)
    {
        if (begin >= str.len())
            return {};

        usize rbegin = 0;
        for (Rune r : iterRunes(str)) {
            if (begin) {
                begin--;
                rbegin += Encoding::getRuneLength(r);
            } else
                break;
        }

        return Substring(str, rbegin, str.len());
    }

    inline StringView Substring(StringView str, Sliceable<Unit> auto& begin)
    {
        int i = IndexOf(str, begin);
        if (i == -1)
            return {};

        return Substring(str, i, str.len());
    }

    // String Substring(Sliceable<Unit> auto& str, usize begin, usize end);

    // String Substring(Sliceable<Unit> auto& str, usize begin);

    // String Substring(Sliceable<Unit> auto& str, Sliceable<Unit> auto& begin);

    inline bool StartsWith(Sliceable<Unit> auto& string,
                           Sliceable<Unit> auto& prefix)
    {
        if (prefix.len() > string.len())
            return false;

        for (usize i = 0; i < prefix.len(); i++) {
            if (string[i] != prefix[i])
                return false;
        }
        return true;
    }

    inline bool StartsWith(Sliceable<Unit> auto& string, const char* prefix)
    {
        return StartsWith(string, StringView{ prefix });
    }

    inline bool StartsWith(Sliceable<Unit> auto& string, Rune prefix)
    {
        Rune r;
        return Encoding::decodeUnit(r, string) && r == prefix;
    }

    inline bool EndsWith(Sliceable<Unit> auto& string,
                         Sliceable<Unit> auto& suffix)
    {
        if (suffix.len() > string.len())
            return false;

        for (usize i = 0; i < suffix.len(); i++) {
            if (string[string.len() - suffix.len() + i] != suffix[i])
                return false;
        }
        return true;
    }

    inline bool EndsWith(Sliceable<Unit> auto& string, const char* suffix)
    {
        return EndsWith(string, StringView{ suffix });
    }

    inline bool EndsWith(Sliceable<Unit> auto& str, Rune suffix)
    {
        Encoding::One unit{};
        if (not Encoding::encodeUnit(suffix, unit))
            return false;

        usize len = str.len() - unit.len();
        for (usize i = len; i < str.len(); i++) {
            if (str[i] != unit[i - len])
                return false;
        }
        return true;
    }

    inline bool Equals(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs)
    {
        if (lhs.len() != rhs.len())
            return false;

        for (usize i = 0; i < lhs.len(); i++) {
            if (lhs[i] != rhs[i])
                return false;
        }
        return true;
    }

    inline bool Equals(Sliceable<Unit> auto& lhs, const char* rhs)
    {
        usize len = lhs.len();
        for (usize i = 0; i < len; i++) {
            if (lhs[i] != rhs[i])
                return false;
        }
        return true;
    }

    inline bool Equals(const char* lhs, Sliceable<Unit> auto& rhs)
    {
        return Equals(rhs, lhs);
    }

    inline bool Equals(const char* lhs, const char* rhs)
    {
        return lhs == rhs || (StringView{ lhs } == StringView{ rhs });
    }

    inline int IndexOf(Sliceable<Unit> auto& str,
                       Sliceable<Unit> auto& substring)
    {
        for (usize i = 0; i < str.len(); i++) {
            for (usize j = 0; j < substring.len(); j++) {
                if (str[i + j] != substring[j])
                    break;
                if (j == substring.len() - 1)
                    return i;
            }
        }
        return -1;
    }

    inline int IndexOf(Sliceable<Unit> auto& str, const char* substring)
    {
        StringView sub{ substring };
        return IndexOf(str, sub);
    }

    inline int LastIndexOf(Sliceable<Unit> auto& str, Rune code)
    {
        int i = 0;
        for (Rune r : iterRunes(str)) {
            if (r == code)
                return i;
        }
        return -1;
    }

    inline int LastIndexOf(Sliceable<Unit> auto& str,
                           Sliceable<Unit> auto& substring)
    {
        for (int i = str.len() - substring.len(); i >= 0; i--) {
            for (usize j = 0; j < substring.len(); j++) {
                if (str[i + j] != substring[j])
                    break;
                if (j == substring.len() - 1)
                    return i;
            }
        }
        return -1;
    }

    inline int LastIndexOf(Sliceable<Unit> auto& str, const char* substring)
    {
        StringView sub{ substring };
        return LastIndexOf(str, sub);
    }

    String Replace(Sliceable<Unit> auto& str,
                   Sliceable<Unit> auto& target,
                   Sliceable<Unit> auto& replacement);

    String ToUpperCase(Sliceable<Unit> auto& str);

    String ToLowerCase(Sliceable<Unit> auto& str);

    String Trim(Sliceable<Unit> auto& str);

    StringView Trim(StringView str);

    String TrimStart(Sliceable<Unit> auto& str);

    StringView TrimStart(StringView str);

    String TrimEnd(Sliceable<Unit> auto& str);

    StringView TrimEnd(StringView str);

    String operator+(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs);

    String operator+(Sliceable<Unit> auto& lhs, const char* rhs);

    String operator+(const char* lhs, Sliceable<Unit> auto& rhs);
}