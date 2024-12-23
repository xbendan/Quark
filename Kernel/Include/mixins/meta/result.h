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
    always_inline constexpr Ok(Args&&... args)
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

    always_inline constexpr Res(Ok<V> const& ok)
        : _inner(ok)
    {
    }

    always_inline constexpr Res(Ok<V>&& ok)
        : _inner(Std::move(ok))
    {
    }

    always_inline constexpr Res(E err)
        : _inner(err)
    {
    }

    template <typename U>
    always_inline constexpr Res(Res<U, E> other)
        : _inner(other._inner)
    {
    }

    constexpr ~Res() {}

    /* --- Operators --- */

    always_inline constexpr explicit operator bool() const
    {
        return _inner.template is<Ok<V>>();
    }

    /* --- Methods --- */

    always_inline constexpr bool isError() const
    {
        return _inner.template is<E>();
    }

    always_inline constexpr bool isSuccess() const
    {
        return _inner.template is<Ok<V>>();
    }

    always_inline constexpr V& unwrap(
        const char* msg = "called `Result::unwrap()` on an error")
    {
        if (!_inner.template is<Ok<V>>()) {
            Std::panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr const V& Unwrap(
        const char* msg = "called `Result::unwrap()` on an error") const
    {
        if (!_inner.template is<Ok<V>>()) {
            Std::panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr const E& Err() const
    {
        if (!_inner.template is<E>()) {
            Std::panic("called `Result::error()` on an ok value");
        }
        return _inner.template unwrap<E>();
    }

    template <typename T>
    always_inline constexpr Res<T, E> Select(Func<T(V)> mapper)
    {
        if (_inner.template is<Ok<V>>()) {
            return Ok(mapper(_inner.template unwrap<Ok<V>>().inner));
        } else {
            return _inner.template unwrap<E>();
        }
    }
};

namespace Qk {
    template <typename TSuccess = Empty, typename... TEx>
    struct Result
    {
        using InnerType = Union<Ok<TSuccess>, TEx...>;

        InnerType _inner;

        always_inline constexpr Result(Ok<TSuccess> const& ok)
            : _inner(ok)
        {
        }

        always_inline constexpr Result(Ok<TSuccess>&& ok)
            : _inner(Std::move(ok))
        {
        }

        template <Any<TEx...> TErrorType>
        always_inline constexpr Result(TErrorType const& err)
            : _inner(err)
        {
        }

        template <typename U>
        always_inline constexpr Result(Result<U, TEx...> other)
            : _inner(other._inner)
        {
        }

        always_inline constexpr ~Result() {}

        always_inline constexpr explicit operator bool() const
        {
            return _inner.template is<Ok<TSuccess>>();
        }

        always_inline constexpr bool isError() const
        {
            return not _inner.template is<Ok<TSuccess>>();
        }

        template <Any<TEx...> TErrorType>
        always_inline constexpr bool isError() const
        {
            return _inner.template is<TErrorType>();
        }

        always_inline constexpr bool isSuccess() const
        {
            return _inner.template is<Ok<TSuccess>>();
        }

        always_inline constexpr TSuccess& unwrap(
            const char* msg = "called `Result::unwrap()` on an error")
        {
            if (not _inner.template is<Ok<TSuccess>>()) {
                indexCast<TEx...>(_inner._index - 1,
                                  _inner._data,
                                  []<typename T>(T& value) {});
            }
            return _inner.template unwrap<Ok<TSuccess>>().inner;
        }

        template <Any<TEx...> Ex>
        always_inline constexpr void catch$(auto f)
        {
            if (_inner.template is<Ok<TSuccess>>()) {
                return;
            }

            if (not _inner.template is<Ok<TSuccess>>()) {
                indexCast<TEx...>(
                    _inner._index - 1, _inner._data, [f]<typename T>(T& value) {
                        if constexpr (Derived<Ex, T>) {
                            f(value);
                        }
                    });
            }
        }
    };

    template <typename TSuccess>
    Result(TSuccess) -> Result<TSuccess, Error>;
}

struct IOException
{};

struct IllegalArgumentException
{};

struct IllegalStateException
{};

struct UnsupportedOperationException
{};

struct IndexOutOfBoundsException
{};

struct CheckFailureException
{};