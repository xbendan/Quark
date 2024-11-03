#pragma once

#include <mixins/meta/iter.h>
#include <mixins/std/concepts.h>

namespace Qk {
    template <typename T, typename U = typename T::InnerType>
    concept Sliceable = requires(T const& t) {
        typename T::InnerType;
        { t.len() } -> SameAs<usize>;
        { t.buf() } -> SameAs<U const*>;
        requires SameAs<decltype(t[0uz]), U const&>;
    };

    template <typename T>
    struct Slice
    {
        using InnerType = T;

        T const* _buf{};
        usize    _len{};

        constexpr Slice() = default;

        constexpr Slice(T const* buf, usize len)
            : _buf(buf)
            , _len(len)
        {
        }

        constexpr Slice(T const* buf, T const* end)
            : _buf(buf)
            , _len(end - buf)
        {
        }

        constexpr Slice(Sliceable<T> auto& other)
            : _buf(other.buf())
            , _len(other.len())
        {
        }

        constexpr T const& operator[](usize i) const { return _buf[i]; }

        constexpr T const* buf() const { return _buf; }

        constexpr usize len() const { return _len; }

        constexpr T const* begin() const { return _buf; }

        constexpr T const* end() const { return _buf + _len; }

        template <typename U>
        constexpr Slice<U> cast() const
        {
            static_assert(sizeof(U) == sizeof(T));
            return Slice<U>(reinterpret_cast<U const*>(_buf), _len);
        }
    };

    template <Sliceable S>
    constexpr auto iter(S const& slice)
    {
        return Iter([&slice, i = 0uz] mutable -> typename S::InnerType const* {
            if (i >= slice.len()) {
                return nullptr;
            }

            return &slice.buf()[i++];
        });
    }
}

using Bytes = Qk::Slice<byte>;