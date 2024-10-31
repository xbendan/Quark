#pragma once

#include <mixins/meta/slice.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/str/encode.h>
#include <mixins/str/string_view.h>
#include <mixins/utils/array.h>

template <EncodingFormat E = Utf8>
struct _String
{
    using Encoding  = E;
    using Unit      = typename E::Unit;
    using InnerType = Unit;

    constexpr _String() = default;

    always_inline constexpr _String(Move, Unit const* buf, usize len)
        : _buf(buf)
        , _len(len)
    {
    }

    always_inline constexpr _String(Unit const* buf, usize len)
        : _len(len)
    {
        if (len == 0) {
            return;
        }

        auto store = new Unit[len + 1];
        store[len] = 0;
        memcpy(store, buf, len);
        _buf = store;
    }

    constexpr _String(const char* buf)
        : _String(Move{}, buf, strlen(buf))
    {
    }

    always_inline _String(Sliceable<Unit> auto& other)
        : _buf(other.buf())
        , _len(other.len())
    {
    }

    always_inline _String(_String const& other)
        : _String(other._buf, other._len)
    {
    }

    always_inline _String(_String&& other)
        : _buf(Std::exchange(other._buf, nullptr))
        , _len(Std::exchange(other._len, 0))
    {
    }

    ~_String()
    {
        if (_buf) {
            _len = 0;
            delete[] Std::exchange(_buf, nullptr);
        }
    }

    always_inline _String& operator=(Sliceable<Unit> auto& other)
    {
        if (_buf) {
            delete[] _buf;
        }

        _buf = other.buf();
        _len = other.len();
        return *this;
    }

    always_inline _String& operator=(Unit const* other)
    {
        if (_buf) {
            delete[] _buf;
        }

        _len       = strlen(other);
        _buf       = new Unit[_len + 1];
        _buf[_len] = 0;
        memcpy(_buf, other, _len);

        return *this;
    }

    always_inline _String& operator=(_String&& other)
    {
        if (_buf) {
            delete[] _buf;
        }

        _buf = Std::exchange(other._buf, nullptr);
        _len = Std::exchange(other._len, 0);
        return *this;
    }

    // always_inline Unit& operator[](usize i)
    // {
    //     MakeAssertion(i < _len, "Index out of bounds");
    //     return _buf[i];
    // }

    always_inline Unit const& operator[](usize i) const
    {
        MakeAssertion(i < _len, "Index out of bounds");
        return _buf[i];
    }

    always_inline constexpr bool operator==(
        Sliceable<Unit> auto const& other) const
    {
        if (_len != other.len()) {
            return false;
        }

        for (usize i = 0; i < _len; i++) {
            if (_buf[i] != other[i]) {
                return false;
            }
        }

        return true;
    }

    always_inline constexpr bool operator==(Unit const* other) const
    {
        return view() == StringView(other);
    }

    always_inline constexpr explicit operator bool() const { return _len > 0; }

    StringView view() const { return StringView(_buf, _len); }

    Unit const* buf() const { return _buf; }

    usize len() const { return _len; }

    Unit const* _buf = nullptr;
    usize       _len = 0;
};

static_assert(Sliceable<_String<Utf8>>);

using String = _String<Utf8>;