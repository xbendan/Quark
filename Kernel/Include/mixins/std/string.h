#pragma once

#include <mixins/meta/slice.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/std/type_traits.h>
#include <mixins/utils/iterator.h>

static constexpr u32 InvalidSymbol = U'�';

template <typename T>
concept EncodingFormat = requires(T t, u32& r, typename T::Unit unit) {
    { T::getUnitLength(unit) } -> SameAs<u8>;
    { T::getRuneLength(r) } -> SameAs<u8>;
};

template <typename T, typename U>
concept EncodeOutput = requires(T t, U u) {
    { t.out(u) } -> ConvertibleTo<void>;
    { t.rem() } -> ConvertibleTo<u8>;
};

template <typename T, typename U>
concept DecodeInput = requires(T t, U u) {
    { t.in() } -> ConvertibleTo<U>;
    { t.rem() } -> ConvertibleTo<u8>;
};

template <typename U, usize Len>
struct _Multiple
{
    using Unit = U;

    Unit  _data[Len];
    usize _len = {};

    void out(Unit u)
    {
        if (_len == Len) {
            return;
            // panic
        }
        _data[_len++] = u;
    }

    constexpr Unit& operator[](usize i) { return _data[i]; }

    constexpr const Unit& operator[](usize i) const { return _data[i]; }

    constexpr usize length() const { return _len; }

    constexpr usize rem() const { return Len - _len; }
};

template <typename U>
struct _Single
{
    using Unit = U;

    Unit _data;

    void out(Unit u) { _data = u; }

    constexpr Unit& operator[](usize) { return _data; }

    constexpr const Unit& operator[](usize) const { return _data; }

    constexpr usize rem() const { return 1; }
};

struct Utf8
{
    using Unit = u8;
    using One  = _Multiple<Unit, 4>;

    /**
     * @brief Get the byte length of specific char code
     *
     * This function returns how many bytes are needed for
     * current unit to represent a character. If the unit
     * is not a valid utf-8 unit, 0 will be returned.
     *
     * Compare the first unit with specific masks, if the
     * unit start with 0, it is a single byte character,
     * this is for the compatibility with ASCII. If the
     * unit start with 0xC0 (110 in binary format), it is
     * a two bytes character. The same rule applies to
     * three bytes (0xE0, 1110 in binary) and four bytes
     * character. (0xF0, 11110 in binary)
     *
     * According to the early design of utf-8, the maximum
     * length of a character is 6 bytes, but the range
     * shrunk to 4 bytes by RFC 3629 in 2003. So we only
     * need to handle with 1 to 4 bytes character.
     *
     * @param unit the first byte of the character
     * @return byte length of current character
     */
    static constexpr u8 getUnitLength(Unit unit)
    {
        if ((unit & 0b1000'0000) == 0x00) {
            return 1;
        } else if ((unit & 0b1110'0000) == 0b1100'0000) {
            return 2;
        } else if ((unit & 0b1111'0000) == 0b1110'0000) {
            return 3;
        } else if ((unit & 0b1111'1000) == 0b1111'0000) {
            return 4;
        } else
            return 0;
    }

    /**
     * @brief Get the rune length of specific rune
     *
     * Rune is an unicode character, it is a 32-bit integer
     * that represents a character. The rune length is the
     * byte length of the rune.
     *
     * The rune under 0x7F is a single byte character, the
     * rune under 0x7FF is a two bytes character, and the
     * same rule applies to three bytes (0xFFFF) and four
     * bytes (more).
     *
     * @param rune
     * @return rune length
     */
    static constexpr u8 getRuneLength(u32 rune)
    {
        if (rune < 0x80) {
            return 1;
        } else if (rune < 0x800) {
            return 2;
        } else if (rune < 0x10000) {
            return 3;
        } else if (rune < 0x110000) {
            return 4;
        } else
            return 0;
    }

    static bool encodeUnit(u32 r, EncodeOutput<Unit> auto& unit)
    {
        if (getUnitLength(r) > unit.rem()) [[unlikely]]
            return false;

        if (r <= 0b0111'1111) {
            unit.out(r);
        } else if (r <= 0x7ff) {
            unit.out(0b1100'0000 | ((r >> 6) & 0b0001'1111));
            unit.out(0b1000'0000 | (r & 0b0011'1111));
        } else if (r <= 0xffff) {
            unit.out(0b1110'0000 | ((r >> 12) & 0b0000'1111));
            unit.out(0b1000'0000 | ((r >> 6) & 0b0011'1111));
            unit.out(0b1000'0000 | (r & 0b0011'1111));
        } else if (r <= 0x10ffff) {
            unit.out(0b1111'0000 | ((r >> 18) & 0b0000'0111));
            unit.out(0b1000'0000 | ((r >> 12) & 0b0011'1111));
            unit.out(0b1000'0000 | ((r >> 6) & 0b0011'1111));
            unit.out(0b1000'0000 | (r & 0b0011'1111));
        } else {
            return encodeUnit(InvalidSymbol, unit);
        }

        return true;
    }

    static bool decodeUnit(u32& r, DecodeInput<Unit> auto& unit)
    {
        if (unit.rem() == 0) {
            r = InvalidSymbol;
            return false;
        }
        Unit u = unit.in();

        if ((u & 0b1000'0000) == 0x00) {
            r = u;
        } else if ((u & 0b1110'0000) == 0b1100'0000) {
            r = (u & 0b0001'1111) << 6;
            r |= unit.in() & 0b0011'1111;
        } else if ((u & 0b1111'0000) == 0b1110'0000) {
            r = (u & 0b0000'1111) << 12;
            r |= (unit.in() & 0b0011'1111) << 6;
            r |= unit.in() & 0b0011'1111;
        } else if ((u & 0b1111'1000) == 0b1111'0000) {
            r = (u & 0b0000'0111) << 18;
            r |= (unit.in() & 0b0011'1111) << 12;
            r |= (unit.in() & 0b0011'1111) << 6;
            r |= unit.in() & 0b0011'1111;
        } else {
            r = u;
        }

        return true;
    }
};

struct Utf16
{
    using Unit = u16;
    using One  = _Multiple<Unit, 2>;

    static constexpr u8 getUnitLength(Unit unit)
    {
        if ((unit & 0xFC00) == 0xD800) {
            return 2;
        } else
            return 1;
    }

    static constexpr u8 getRuneLength(u32 rune)
    {
        if (rune < 0x10000) {
            return 1;
        } else if (rune < 0x110000) {
            return 2;
        } else
            return 0;
    }

    static bool encodeUnit(u32 r, EncodeOutput<Unit> auto& unit)
    {
        if (r <= 0xffff) {
            unit.out(r);
            return true;
        } else if (r <= 0x10ffff) {
            unit.out(0xD800 | ((r - 0x10000) >> 10));
            unit.out(0xDC00 | ((r - 0x10000) & 0x3FF));
            return true;
        }
        return encodeUnit(InvalidSymbol, unit);
    }

    static bool decodeUnit(u32& r, DecodeInput<Unit> auto& unit)
    {
        Unit u = unit.in();

        if (getUnitLength(u) > unit.rem()) {
            r = InvalidSymbol;
            return false;
        }

        if (u >= 0xd800 && u <= 0xdbff) {
            if (unit.rem() < 2) {
                return false;
            }
            Unit u2 = unit.in();
            if (u2 < 0xdc00 || u2 > 0xdfff) {
                return false;
            }

            r = 0x10000 + ((u & 0x3ff) << 10) + (u2 & 0x3ff);
        } else {
            r = u;
        }

        return true;
    }
};

struct Ascii
{
    using Unit = u8;
    using Code = _Single<Unit>;

    static constexpr u8 getUnitLength(Unit) { return 1; }

    static constexpr u8 getRuneLength(u32) { return 1; }

    static bool encodeUnit(u32 r, EncodeOutput<Unit> auto& unit)
    {
        if (r > 0x7f) {
            unit.out('?');
            return false;
        }
        unit.out(r);
        return true;
    }

    static bool decodeUnit(u32& r, DecodeInput<Unit> auto& unit)
    {
        if (unit.rem() == 0) {
            r = '?';
            return false;
        }
        r = unit.in();
        return true;
    }
};

template <EncodingFormat E = Utf8, typename U = typename E::Unit>
class String : public Slice<U>
{
public:
    using Format = E;
    using Unit   = E::Unit;

    [[gnu::always_inline]]
    constexpr String() = default;

    template <typename TCode = u8>
    [[gnu::always_inline]]
    constexpr String(TCode const* buf)
        : Slice<U>((Unit const*)buf, LengthOf(buf))
    {
    }

    [[gnu::always_inline]]
    constexpr String(Unit const* buf, usize len)
        : Slice<U>(buf, len)
    {
    }

    [[gnu::always_inline]]
    constexpr String(Unit const* begin, Unit const* end)
        : Slice<U>(begin, end - begin)
    {
    }

    [[gnu::always_inline]]
    constexpr String(Sliceable<U> auto& other)
        : Slice<U>(other.Data(), other.Length())
    {
    }

    [[gnu::always_inline]]
    constexpr bool operator==(Sliceable<U> auto const& other) const
        requires(SameAs<U, typename decltype(other)::InnerType>)
    {
        for (usize i = 0; i < other.len(); i++) {
            if (this->buf()[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    [[gnu::always_inline]]
    constexpr bool operator==(Unit const* other) const
    {
        return *this == String(other);
    }

    template <typename T = char>
        requires(ConvertibleTo<Unit, T>)
    [[gnu::always_inline]]
    constexpr bool operator==(T const* other) const
    {
        return *this == String(other);
    }

    [[gnu::always_inline]]
    constexpr bool operator==(String const& other) const
    {
        if (this->Length() != other.Length()) {
            return false;
        }

        if (this->Data() != other.Data()) {
            for (usize i = 0; i < this->Length(); i++) {
                if (this->Data()[i] != other[i]) {
                    return false;
                }
            }
        }
        return true;
    }

    [[gnu::always_inline]]
    constexpr bool Equals(char const* other)
    {
        return *this == String(other);
    }

    [[gnu::always_inline]]
    constexpr bool IsEmpty() const
    {
        return this->Length() == 0;
    }

    constexpr String<E> Substring(usize begin, usize end)
    {
        return String<E>(&(this->Data()[begin]), end - begin);
    }

    constexpr String<E> Substring(usize begin)
    {
        return Substring(begin, this->Length());
    }

    constexpr int IndexOf(Unit u) const
    {
        for (usize i = 0; i < this->Length(); i++) {
            if (this->Data()[i] == u) {
                return i;
            }
        }
        return -1;
    }

    constexpr int IndexOf(String<E> const& other) const
    {
        if (other.Length() > this->Length()) {
            return -1;
        }

        for (usize i = 0; i < this->Length(); i++) {
            if (this->Data()[i] == other[0]) {
                bool found = true;
                for (usize j = 1; j < other.Length(); j++) {
                    if (this->Data()[i + j] != other[j]) {
                        found = false;
                        break;
                    }
                }
                if (found) {
                    return i;
                }
            }
        }
        return -1;
    }

    template <typename T = char>
    static constexpr usize LengthOf(T const* str)
    {
        usize len = 0;
        while (str[len] != '\0') {
            len++;
        }
        return len;
    }

    template <typename T = char>
    static constexpr void Copy(T const* src, T* dst)
    {
        usize i = 0;
        while (src[i] != '\0') {
            dst[i] = src[i];
            i++;
        }
    }
};

static_assert(EncodingFormat<Utf8>);
static_assert(EncodingFormat<Utf16>);
static_assert(EncodingFormat<Ascii>);

using string = String<Utf8>;