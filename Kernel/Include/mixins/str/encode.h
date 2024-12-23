#pragma once

#include <mixins/meta/buf.h>
#include <mixins/meta/cursor.h>
#include <mixins/meta/iter.h>
#include <mixins/meta/opt.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>

namespace Qk {
    using Rune = u32;

    static constexpr Rune INVALID_SYMBOL = U'�';

    template <typename T, typename U>
    concept EncodeOutput = requires(T t, U u) {
        { t.put(u) };
    };

    template <typename T, typename U>
    concept DecodeInput = requires(T t, U u) {
        { t.next() } -> ConvertibleTo<U>;
        { t.rem() } -> ConvertibleTo<u8>;
    };

    template <typename T>
    concept EncodingFormat = requires(T                         t,
                                      Rune&                     r,
                                      typename T::Unit          unit,
                                      Cursor<typename T::Unit>& c) {
        { T::getUnitLength(unit) } -> SameAs<u8>;
        { T::getRuneLength(r) } -> SameAs<u8>;
        { T::decodeUnit(r, c) } -> SameAs<bool>;
        { T::encodeUnit(Rune{}, c) } -> SameAs<bool>;
    };

    template <typename U, usize N>
    struct _Multiple
    {
        using InnerType = U;
        using Unit      = U;

        Buf<Unit, N> _buf{};

        always_inline void put(Unit u) { _buf.emplace(_buf.len(), u); }

        always_inline constexpr Unit& operator[](usize i) { return _buf[i]; }

        always_inline constexpr Unit const& operator[](usize i) const
        {
            return _buf[i];
        }

        always_inline constexpr Unit* buf() { return _buf.buf(); }

        always_inline constexpr Unit const* buf() const { return _buf.buf(); }

        always_inline constexpr usize len() const { return _buf.len(); }

        always_inline constexpr usize rem() const { return N - len(); }
    };

    template <typename U>
    struct _Single
    {
        using InnerType = U;
        using Unit      = U;

        Unit _buf;

        always_inline _Single() = default;

        always_inline _Single(Unit u)
            : _buf(u)
        {
        }

        always_inline void put(Unit u) { _buf = u; }

        always_inline operator Unit() { return _buf; }

        always_inline constexpr Unit& operator[](usize) { return _buf; }

        always_inline constexpr Unit const& operator[](usize) const
        {
            return _buf;
        }

        always_inline constexpr Unit* buf() { return &_buf; }

        always_inline constexpr Unit const* buf() const { return &_buf; }

        always_inline constexpr usize len() const { return 1; }
    };

    namespace Encoding {
        struct UTF8
        {
            using Unit = char;
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
            static constexpr u8 getRuneLength(Rune rune)
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

            static bool encodeUnit(Rune r, EncodeOutput<Unit> auto& unit)
            {
                if (getUnitLength(r) > unit.rem()) [[unlikely]]
                    return false;

                if (r <= 0b0111'1111) {
                    unit.put(r);
                } else if (r <= 0x7ff) {
                    unit.put(0b1100'0000 | ((r >> 6) & 0b0001'1111));
                    unit.put(0b1000'0000 | (r & 0b0011'1111));
                } else if (r <= 0xffff) {
                    unit.put(0b1110'0000 | ((r >> 12) & 0b0000'1111));
                    unit.put(0b1000'0000 | ((r >> 6) & 0b0011'1111));
                    unit.put(0b1000'0000 | (r & 0b0011'1111));
                } else if (r <= 0x10ffff) {
                    unit.put(0b1111'0000 | ((r >> 18) & 0b0000'0111));
                    unit.put(0b1000'0000 | ((r >> 12) & 0b0011'1111));
                    unit.put(0b1000'0000 | ((r >> 6) & 0b0011'1111));
                    unit.put(0b1000'0000 | (r & 0b0011'1111));
                } else {
                    return encodeUnit(INVALID_SYMBOL, unit);
                }

                return true;
            }

            static bool decodeUnit(Rune& r, DecodeInput<Unit> auto& unit)
            {
                if (unit.rem() == 0) {
                    r = INVALID_SYMBOL;
                    return false;
                }
                Unit u = unit.next();

                if ((u & 0b1000'0000) == 0x00) {
                    r = u;
                } else if ((u & 0b1110'0000) == 0b1100'0000) {
                    r = (u & 0b0001'1111) << 6;
                    r |= unit.next() & 0b0011'1111;
                } else if ((u & 0b1111'0000) == 0b1110'0000) {
                    r = (u & 0b0000'1111) << 12;
                    r |= (unit.next() & 0b0011'1111) << 6;
                    r |= unit.next() & 0b0011'1111;
                } else if ((u & 0b1111'1000) == 0b1111'0000) {
                    r = (u & 0b0000'0111) << 18;
                    r |= (unit.next() & 0b0011'1111) << 12;
                    r |= (unit.next() & 0b0011'1111) << 6;
                    r |= unit.next() & 0b0011'1111;
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

            static constexpr u8 getRuneLength(Rune rune)
            {
                if (rune < 0x10000) {
                    return 1;
                } else if (rune < 0x110000) {
                    return 2;
                } else
                    return 0;
            }

            static bool encodeUnit(Rune r, EncodeOutput<Unit> auto& unit)
            {
                if (r <= 0xffff) {
                    unit.put(r);
                    return true;
                } else if (r <= 0x10ffff) {
                    unit.put(0xD800 | ((r - 0x10000) >> 10));
                    unit.put(0xDC00 | ((r - 0x10000) & 0x3FF));
                    return true;
                }
                return encodeUnit(INVALID_SYMBOL, unit);
            }

            static bool decodeUnit(Rune& r, DecodeInput<Unit> auto& unit)
            {
                Unit u = unit.next();

                if (getUnitLength(u) > unit.rem()) {
                    r = INVALID_SYMBOL;
                    return false;
                }

                if (u >= 0xd800 && u <= 0xdbff) {
                    if (unit.rem() < 2) {
                        return false;
                    }
                    Unit u2 = unit.next();
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
            using Unit = char;
            using Code = _Single<Unit>;

            static constexpr u8 getUnitLength(Unit) { return 1; }

            static constexpr u8 getRuneLength(Rune) { return 1; }

            static bool encodeUnit(Rune r, EncodeOutput<Unit> auto& unit)
            {
                if (r > 0x7f) {
                    unit.put('?');
                    return false;
                }
                unit.put(r);
                return true;
            }

            static bool decodeUnit(Rune& r, DecodeInput<Unit> auto& unit)
            {
                if (unit.rem() == 0) {
                    r = '?';
                    return false;
                }
                r = unit.next();
                return true;
            }
        };

        static_assert(EncodingFormat<Encoding::UTF8>);
        static_assert(EncodingFormat<Utf16>);
        static_assert(EncodingFormat<Ascii>);
    }

    template < //
        Sliceable S,
        typename E = typename S::Encoding,
        typename U = E::Unit>
    auto iterRunes(S const& slice)
    {
        Cursor<U> cursor(slice);
        return Iter([cursor] mutable -> Optional<Rune> {
            if (cursor.ended()) {
                return Empty{};
            }

            Rune r;
            return E::decodeUnit(r, cursor) ? Optional<Rune>(r)
                                            : Optional<Rune>();
        });
    }
}