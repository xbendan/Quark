#include <mixins/str/string_builder.h>
#include <mixins/utils/arrays.h>
#include <mixins/utils/chars.h>
#include <mixins/utils/strings.h>

namespace Qk::Strings {
    // String Concat(Sliceable<Unit> auto& begin, Sliceable<Unit> auto& end)
    // {
    //     StringBuilder<> result(begin.len() + end.len() + 1);
    //     result.append(begin);
    //     result.append(end);
    //     return result.getString();
    // }

    // String Concat(Sliceable<Unit> auto& begin, const char* end)
    // {
    //     return Concat(begin, StringView{ end });
    // }

    // String Concat(const char* begin, Sliceable<Unit> auto& end)
    // {
    //     return Concat(StringView{ begin }, end);
    // }

    // // template <>
    // // bool Contains(StringView string, Rune rune)
    // // {
    // //     for (Rune r : iterRunes(string)) {
    // //         if (r == rune)
    // //             return true;
    // //     }
    // //     return false;
    // // }

    // // template <>
    // // bool Contains(String string, Rune rune)
    // // {
    // //     for (Rune r : iterRunes(string)) {
    // //         if (r == rune)
    // //             return true;
    // //     }
    // //     return false;
    // // }

    // template <>
    // StringView Substring(StringView& string, usize begin, usize end)
    // {
    //     MakeAssertion(begin <= end && end <= string.len(),
    //                   Error::IndexOutOfBounds("Invalid range"));

    //     usize rbegin = 0, rend = 0;
    //     usize len;
    //     for (Rune r : iterRunes(string)) {
    //         len = Encoding::getRuneLength(r);
    //         if (begin) {
    //             begin--;
    //             rbegin += len;
    //         }

    //         if (end--) {
    //             rend += len;
    //         } else
    //             break;
    //     }
    //     return { string.buf() + rbegin, rend - rbegin };
    // }

    // template <>
    // StringView Substring(StringView& string, usize begin)
    // {
    //     if (begin >= string.len())
    //         return {};

    //     usize rbegin = 0;
    //     for (Rune r : iterRunes(string)) {
    //         if (begin) {
    //             begin--;
    //             rbegin += Encoding::getRuneLength(r);
    //         } else
    //             break;
    //     }

    //     return Substring(string, rbegin, string.len());
    // }

    // template <>
    // StringView Substring(StringView& string, StringView& begin)
    // {
    //     int i = IndexOf(string, begin);
    //     if (i == -1)
    //         return {};

    //     return Substring(string, i, string.len());
    // }

    // template <>
    // String Substring(String& string, usize begin, usize end)
    // {
    //     MakeAssertion(begin <= end && end <= string.len(),
    //                   Error::IndexOutOfBounds("Invalid range"));

    //     StringBuilder<> result(end - begin + 1);
    //     usize           i = 0;
    //     for (Rune r : iterRunes(string)) {
    //         if (i >= begin && i < end)
    //             result.append(r);
    //         i++;
    //     }

    //     return result.getString();
    // }

    // template <>
    // String Substring(String& string, usize begin)
    // {
    //     return Substring(string, begin, string.len());
    // }

    // template <>
    // String Substring(String& string, String& begin)
    // {
    //     int i = IndexOf(string, begin);
    //     if (i == -1)
    //         return {};

    //     return Substring(string, i, string.len());
    // }

    // bool StartsWith(Sliceable<Unit> auto& string, Sliceable<Unit> auto&
    // prefix)
    // {
    //     if (prefix.len() > string.len())
    //         return false;

    //     for (usize i = 0; i < prefix.len(); i++) {
    //         if (string[i] != prefix[i])
    //             return false;
    //     }
    //     return true;
    // }

    // bool StartsWith(Sliceable<Unit> auto& string, const char* prefix)
    // {
    //     return StartsWith(string, StringView{ prefix });
    // }

    // bool StartsWith(Sliceable<Unit> auto& string, Rune prefix)
    // {
    //     Rune r;
    //     return Encoding::decodeUnit(r, string) && r == prefix;
    // }

    // bool EndsWith(Sliceable<Unit> auto& string, Sliceable<Unit> auto& suffix)
    // {
    //     if (suffix.len() > string.len())
    //         return false;

    //     for (usize i = 0; i < suffix.len(); i++) {
    //         if (string[string.len() - suffix.len() + i] != suffix[i])
    //             return false;
    //     }
    //     return true;
    // }

    // bool EndsWith(Sliceable<Unit> auto& string, const char* suffix)
    // {
    //     return EndsWith(string, StringView{ suffix });
    // }

    // bool EndsWith(Sliceable<Unit> auto& str, Rune suffix)
    // {
    //     Encoding::One unit{};
    //     if (not Encoding::encodeUnit(suffix, unit))
    //         return false;

    //     usize len = str.len() - unit.len();
    //     for (usize i = len; i < str.len(); i++) {
    //         if (str[i] != unit[i - len])
    //             return false;
    //     }
    //     return true;
    // }

    // bool Equals(Sliceable<Unit> auto& lhs, Sliceable<Unit> auto& rhs)
    // {
    //     if (lhs.len() != rhs.len())
    //         return false;

    //     for (usize i = 0; i < lhs.len(); i++) {
    //         if (lhs[i] != rhs[i])
    //             return false;
    //     }
    //     return true;
    // }

    // bool Equals(Sliceable<Unit> auto& lhs, const char* rhs)
    // {
    //     usize len = lhs.len();
    //     for (usize i = 0; i < len; i++) {
    //         if (lhs[i] != rhs[i])
    //             return false;
    //     }
    //     return true;
    // }

    // bool Equals(const char* lhs, Sliceable<Unit> auto& rhs)
    // {
    //     return Equals(rhs, lhs);
    // }

    // bool Equals(const char* lhs, const char* rhs)
    // {
    //     return lhs == rhs || (StringView{ lhs } == StringView{ rhs });
    // }

    // int IndexOf(Sliceable<Unit> auto& str, Rune rune)
    // {
    //     int i = 0;
    //     for (Rune r : iterRunes(str)) {
    //         if (r == rune)
    //             return i;
    //         i++;
    //     }
    //     return -1;
    // }

    // int IndexOf(Sliceable<Unit> auto& str, Sliceable<Unit> auto& substring)
    // {
    //     for (usize i = 0; i < str.len(); i++) {
    //         for (usize j = 0; j < substring.len(); j++) {
    //             if (str[i + j] != substring[j])
    //                 break;
    //             if (j == substring.len() - 1)
    //                 return i;
    //         }
    //     }
    //     return -1;
    // }

    // int IndexOf(Sliceable<Unit> auto& str, const char* substring)
    // {
    //     StringView sub{ substring };
    //     return IndexOf(str, sub);
    // }

    // int LastIndexOf(Sliceable<Unit> auto& str, Rune code)
    // {
    //     int i = 0;
    //     for (Rune r : iterRunes(str)) {
    //         if (r == code)
    //             return i;
    //     }
    //     return -1;
    // }

    // int LastIndexOf(Sliceable<Unit> auto& str, Sliceable<Unit> auto&
    // substring)
    // {
    //     for (int i = str.len() - substring.len(); i >= 0; i--) {
    //         for (usize j = 0; j < substring.len(); j++) {
    //             if (str[i + j] != substring[j])
    //                 break;
    //             if (j == substring.len() - 1)
    //                 return i;
    //         }
    //     }
    //     return -1;
    // }

    // int LastIndexOf(Sliceable<Unit> auto& str, const char* substring)
    // {
    //     StringView sub{ substring };
    //     return LastIndexOf(str, sub);
    // }

    // String Replace(Sliceable<Unit> auto& str,
    //                Sliceable<Unit> auto& target,
    //                Sliceable<Unit> auto& replacement)
    // {
    //     StringBuilder<> result;
    //     for (usize i = 0; i < str.len(); i++) {
    //         if (memcmp(str.buf() + i, target.buf(), target.len()) == 0) {
    //             result.append(replacement);
    //             i += target.len() - 1;
    //         } else {
    //             result.append(str[i]);
    //         }
    //     }
    //     return result.getString();
    // }

    // String ToUpperCase(Sliceable<Unit> auto& str)
    // {
    //     StringBuilder<> result;
    //     for (usize i = 0; i < str.len(); i++) {
    //         result.append(Chars::ToUpperCase(str[i]));
    //     }
    //     return result.getString();
    // }

    // String ToLowerCase(Sliceable<Unit> auto& str)
    // {
    //     StringBuilder<> result;
    //     for (usize i = 0; i < str.len(); i++) {
    //         result.append(Chars::ToLowerCase(str[i]));
    //     }
    //     return result.getString();
    // }

    // String Trim(Sliceable<Unit> auto& str)
    // {
    //     usize start = 0;
    //     usize end   = str.len();

    //     while (start < end && str[start] == ' ')
    //         start++;
    //     while (end > start && str[end - 1] == ' ')
    //         end--;

    //     return Substring(str, start, end);
    // }

    // String TrimStart(Sliceable<Unit> auto& str)
    // {
    //     usize start = 0;
    //     while (start < str.len() && str[start] == ' ')
    //         start++;
    //     return Substring(str, start);
    // }

    // String TrimEnd(Sliceable<Unit> auto& str)
    // {
    //     usize end = str.len();
    //     while (end > 0 && str[end - 1] == ' ')
    //         end--;
    //     return Substring(str, 0, end);
    // }
}