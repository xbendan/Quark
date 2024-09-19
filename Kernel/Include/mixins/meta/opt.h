#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/union.h>
#include <mixins/std/concepts.h>
#include <mixins/std/panic.h>
#include <mixins/std/utility.h>

template <typename T>
struct Opt;

template <typename T>
struct [[nodiscard]] Opt
{
    union
    {
        T     _value;
        Empty _empty;
    };
    bool _present;

    [[gnu::always_inline]]
    constexpr Opt()
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Opt(Empty)
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Opt(T const& value)
        : _value(value)
        , _present(true)
    {
    }

    template <typename U = T>
    [[gnu::always_inline]]
    constexpr Opt(U&& value)
        requires(!SameAs<Std::RemoveCvRef<U>, Opt<T>> &&
                 MoveConstructible<T, U>)
        : _value(Std::forward<U>(value))
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr Opt(Opt const& other)
        requires CopyConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(other._value);
    }

    [[gnu::always_inline]]
    constexpr Opt(Opt&& other)
        requires MoveConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(Std::move(other._value));
    }

    template <typename... Args>
    [[gnu::always_inline]] constexpr Opt(Args&&... args)
        : _value(Std::forward<Args>(args)...)
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr ~Opt()
    {
        clear();
    }

    [[gnu::always_inline]]
    constexpr Opt& operator=(Empty)
    {
        clear();
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Opt& operator=(T const& value)
    {
        clear();

        _present = true;
        new (&_value) T(Std::forward(value));
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Opt& operator=(T&& value)
    {
        clear();

        _present = true;
        new (&_value) T(Std::move(value));
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Opt& operator=(Opt const& other)
        requires CopyConstructible<T>
    {
        if (this != &other) {
            clear();

            _present = other._present;
            if (_present)
                new (&_value) T(other._value);
        }
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Opt& operator=(Opt&& other)
        requires MoveConstructible<T>
    {
        if (this != &other) {
            clear();

            _present = other._present;
            if (_present)
                new (&_value) T(Std::move(other._value));
        }
        return *this;
    }

    [[gnu::always_inline]]
    void clear()
    {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    [[gnu::always_inline]]
    constexpr explicit operator bool() const
    {
        return _present;
    }

    [[gnu::always_inline]]
    constexpr bool isPresent() const
    {
        return _present;
    }

    [[gnu::always_inline]]
    constexpr void ifPresent(Consumer<T&> f)
    {
        if (_present)
            f(_value);
    }

    [[gnu::always_inline]]
    constexpr T* operator->()
    {
        if (!_present)
            Std::panic("No value present");

        return &_value;
    }

    [[gnu::always_inline]]
    constexpr T& operator*()
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    [[gnu::always_inline]]
    constexpr T const* operator->() const
    {
        if (!_present)
            Std::panic("No value present");

        return &_value;
    }

    [[gnu::always_inline]]
    constexpr T const& operator*() const
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    template <typename... Args>
    [[gnu::always_inline]]
    T& emplace(Args&&... args)
    {
        clear();

        _present = true;
        new (&_value) T(Std::forward<Args>(args)...);
        return _value;
    }

    [[gnu::always_inline]]
    constexpr T& get(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    [[gnu::always_inline]]
    constexpr T const& get(const char* msg = "No value present") const
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    [[gnu::always_inline]]
    constexpr T orElse(T other) const
    {
        return _present ? Std::move(_value) : other;
    }

    [[gnu::always_inline]]
    constexpr T take(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        T v = Std::move(_value);
        clear();
        return v;
    }

    template <typename To>
    [[gnu::always_inline]]
    constexpr Opt<To> map(Func<To(T)> f) const
    {
        if (_present)
            return f(_value);
        return Opt<To>();
    }

    template <typename... Args>
    [[gnu::always_inline]]
    constexpr auto operator()(Args&&... args) const
    {
        using OptRet = Opt<InvokeResult<T, Args...>>;

        if constexpr (SameAs<void, InvokeResult<T, Args...>>) {
            if (!_present) {
                return false;
            }
            _value(Std::forward<Args>(args)...);
            return true;
        } else {
            if (!_present) {
                return OptRet();
            }
            return OptRet(_value(Std::forward<Args>(args)...));
        }
    }
};