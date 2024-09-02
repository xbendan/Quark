#pragma once

#include <mixins/std/concepts.h>

template <typename T, typename U = typename T::InnerType>
concept Sliceable = requires(T const& t) {
    typename T::InnerType;
    { t.size() } -> SameAs<usize>;
    { t.data() } -> SameAs<U const*>;
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

    constexpr T const* data() const { return _buf; }

    constexpr usize size() const { return _len; }

    constexpr T const* begin() const { return _buf; }

    constexpr T const* end() const { return _buf + _len; }

    template <typename U>
    constexpr Slice<U> cast() const
    {
        static_assert(sizeof(U) == sizeof(T));
        return Slice<U>(reinterpret_cast<U const*>(_buf), _len);
    }
};