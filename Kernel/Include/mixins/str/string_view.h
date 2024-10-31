#pragma once

#include <mixins/meta/slice.h>
#include <mixins/std/string.h>
#include <mixins/str/encode.h>

template <EncodingFormat E = Utf8>
struct _StringView
{
    using Encoding  = E;
    using Unit      = typename E::Unit;
    using InnerType = Unit;

    always_inline _StringView() = default;

    constexpr _StringView(const char* buf)
        : _StringView(buf, strlen(buf))
    {
    }

    always_inline constexpr _StringView(Unit const* buf, usize len)
        : _buf(buf)
        , _len(len)
    {
    }

    always_inline constexpr _StringView(Unit const* begin, Unit const* end)
        : _buf(begin)
        , _len(end - begin)
    {
    }

    always_inline _StringView(Sliceable<Unit> auto& other)
        : _buf(other.buf())
        , _len(other.len())
    {
    }

    always_inline _StringView(_StringView const& other)
        : _StringView(other._buf, other._len)
    {
    }

    always_inline _StringView(_StringView&& other)
        : _buf(Std::exchange(other._buf, nullptr))
        , _len(Std::exchange(other._len, 0))
    {
    }

    always_inline _StringView& operator=(Sliceable<Unit> auto& other)
    {
        _buf = other.buf();
        _len = other.len();

        return *this;
    }

    always_inline _StringView& operator=(Unit const* other)
    {
        _buf = other;
        _len = strlen(other);
    }

    always_inline _StringView& operator=(_StringView const& other)
    {
        _buf = other._buf;
        _len = other._len;

        return *this;
    }

    always_inline _StringView& operator=(_StringView&& other)
    {
        _buf = Std::exchange(other._buf, nullptr);
        _len = Std::exchange(other._len, 0);

        return *this;
    }

    always_inline Unit const& operator[](usize i) const { return _buf[i]; }

    always_inline bool operator==(Sliceable<Unit> auto const& other) const
    {
        if (_len != other.len()) {
            return false;
        }

        for (usize i = 0; i < other.len(); i++) {
            if (_buf[i] != other[i]) {
                return false;
            }
        }

        return true;
    }

    always_inline bool operator==(Unit const* other) const
    {
        return *this == _StringView(other);
    }

    always_inline constexpr explicit operator bool() const { return _len > 0; }

    always_inline bool isEmpty() const { return _len == 0; }

    always_inline Unit* begin() const { return _buf; }

    always_inline Unit* end() const { return _buf + _len; }

    always_inline Unit const* buf() const { return _buf; }

    always_inline usize len() const { return _len; }

    Unit const* _buf{};
    usize       _len{};
};

static_assert(Sliceable<_StringView<Utf8>>);

using StringView = _StringView<Utf8>;

constexpr inline StringView
operator""s(const char* str, usize len)
{
    return StringView{ str, len };
}