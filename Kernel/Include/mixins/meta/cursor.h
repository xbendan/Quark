#pragma once

#include <mixins/meta/slice.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/std/panic.h>

namespace Qk {
    template <typename T>
    struct Cursor
    {
        using InnerType = T;

        T* _begin = nullptr;
        T* _end   = nullptr;

        constexpr Cursor() = default;

        always_inline constexpr Cursor(T const* ptr)
            : Cursor(ptr, ptr ? 1 : 0)
        {
        }

        always_inline constexpr Cursor(T const* ptr, usize len)
            : _begin(ptr)
            , _end(ptr + len)
        {
            if (_begin == nullptr and _begin != _end) [[unlikely]]
                Std::panic("null pointer with non-zero length");
        }

        always_inline constexpr Cursor(T const* begin, T const* end)
            : _begin{ const_cast<T*>(begin) }
            , _end{ const_cast<T*>(end) }
        {
        }

        always_inline constexpr Cursor(Sliceable<T> auto const& slice)
            : Cursor{ slice.buf(), slice.buf() + slice.len() }
        {
        }

        always_inline constexpr T& operator[](usize i)
        {
            if (i >= len()) [[unlikely]]
                Std::panic("index out of bounds");
            return _begin[i];
        }

        always_inline constexpr T const& operator[](usize i) const
        {
            if (i >= len()) [[unlikely]]
                Std::panic("index out of bounds");
            return _begin[i];
        }

        always_inline constexpr operator T*() { return _begin; }

        always_inline constexpr operator T const*() const { return _begin; }

        always_inline bool ended() const { return _begin == _end; }

        always_inline constexpr usize rem() const { return _end - _begin; }

        always_inline constexpr T& peek(usize i = 0)
        {
            if (i >= len()) [[unlikely]]
                Std::panic("index out of bounds");
            return _begin[i];
        }

        always_inline constexpr T const& peek(usize i = 0) const
        {
            if (i >= len()) [[unlikely]]
                Std::panic("index out of bounds");
            return _begin[i];
        }

        always_inline constexpr T& operator*() { return peek(); }

        always_inline constexpr T* operator->() { return &peek(); }

        always_inline constexpr T const& operator*() const { return peek(); }

        always_inline constexpr T const* operator->() const { return &peek(); }

        always_inline constexpr T& next()
        {
            if (ended()) [[unlikely]]
                Std::panic("next() called on ended cursor");

            T& r = *_begin;
            _begin++;
            return r;
        }

        always_inline constexpr Slice<T> next(usize n)
        {
            if (n > rem()) [[unlikely]]
                Std::panic("next() called on ended cursor");

            auto slice = Slice<T>{ _begin, n };
            _begin += n;
            return slice;
        }

        always_inline constexpr bool put(T c)
        {
            if (_begin == _end) {
                return true;
            }

            *_begin++ = c;
            return false;
        }

        always_inline constexpr T* buf() { return _begin; }

        always_inline constexpr T const* buf() const { return _begin; }

        always_inline constexpr usize len() const { return _end - _begin; }
    };
}
