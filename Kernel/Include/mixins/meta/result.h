#pragma once

#include <mixins/meta/error.h>
#include <mixins/meta/func.h>
#include <mixins/meta/union.h>
#include <mixins/std/c++types.h>
#include <mixins/std/panic.h>
#include <mixins/std/utility.h>

template <typename T = Empty>
struct Ok
{
    T inner;

    template <typename... Args>
    [[gnu::always_inline]] constexpr Ok(Args&&... args)
        : inner(Std::forward<Args>(args)...)
    {
    }

    T take() { return Std::move(inner); }
};

template <typename... Args>
Ok(Args&&...) -> Ok<Std::RemoveCvRef<Args>...>;

template <typename V = Empty, typename E = Error>
struct Res
{
    using Inner = Union<Ok<V>, E>;

    /* --- Fields --- */

    Inner _inner;

    /* --- Constructors and Destructors --- */

    [[gnu::always_inline]]
    constexpr Res(const Ok<V>& ok)
        : _inner(ok)
    {
    }

    [[gnu::always_inline]]
    constexpr Res(Ok<V>&& ok)
        : _inner(Std::move(ok))
    {
    }

    [[gnu::always_inline]]
    constexpr Res(E err)
        : _inner(err)
    {
    }

    [[gnu::always_inline]]
    constexpr Res(Res<V, E>&& other)
        : _inner(Std::move(other._inner))
    {
    }

    constexpr ~Res() {}

    /* --- Operators --- */

    [[gnu::always_inline]]
    constexpr explicit operator bool() const
    {
        return _inner.template is<Ok<V>>();
    }

    /* --- Methods --- */

    [[gnu::always_inline]]
    constexpr bool has() const
    {
        return _inner.template is<Ok<V>>();
    }

    [[gnu::always_inline]]
    constexpr V& unwrap(
        const char* msg = "called `Result::unwrap()` on an error")
    {
        if (!_inner.template is<Ok<V>>()) {
            Std::panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]]
    constexpr const V& unwrap(
        const char* msg = "called `Result::unwrap()` on an error") const
    {
        if (!_inner.template is<Ok<V>>()) {
            Std::panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]]
    constexpr const E& error() const
    {
        if (!_inner.template is<E>()) {
            Std::panic("called `Result::error()` on an ok value");
        }
        return _inner.template unwrap<E>();
    }

    template <typename T>
    [[gnu::always_inline]]
    constexpr Res<T, E> map(Func<T(V)> mapper)
    {
        if (_inner.template is<Ok<V>>()) {
            return Ok(mapper(_inner.template unwrap<Ok<V>>().inner));
        } else {
            return _inner.template unwrap<E>();
        }
    }
};