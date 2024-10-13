#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/union.h>
#include <mixins/std/concepts.h>
#include <mixins/std/panic.h>
#include <mixins/std/utility.h>

template <typename T>
struct Optional;

template <typename T>
    requires Std::isReference<T>
struct [[nodiscard]] Optional<T>
{
    using TPureType  = Std::RemoveCvRef<T>;
    using TInnerType = Std::AddPointer<TPureType>;

    union
    {
        TInnerType _value;
        Empty      _empty;
    };
    bool _present;

    [[gnu::always_inline]]
    constexpr Optional()
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(Empty)
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(TInnerType value)
        : _value(value)
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(T value)
        : _value(&value)
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(Optional const& other)
        : _present(other._present)
    {
        if (_present)
            _value = other._value;
    }

    [[gnu::always_inline]]
    constexpr Optional(Optional&& other)
        : _present(other._present)
    {
        if (_present)
            _value = other._value;
    }

    [[gnu::always_inline]]
    constexpr ~Optional()
    {
        Clear();
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(Empty)
    {
        Clear();
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(TInnerType value)
    {
        Clear();

        _present = true;
        _value   = value;
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(T value)
    {
        Clear();

        _present = true;
        _value   = &value;
        return *this;
    }

    [[gnu::always_inline]]

    constexpr Optional& operator=(Optional const& other)
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                _value = other._value;
        }
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(Optional&& other)
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                _value = other._value;
        }
        return *this;
    }

    [[gnu::always_inline]]
    void Clear()
    {
        if (_present) {
            _value   = nullptr;
            _present = false;
        }
    }

    [[gnu::always_inline]]
    constexpr explicit operator bool() const
    {
        return _present;
    }

    [[gnu::always_inline]]
    constexpr bool IsPresent() const
    {
        return _present;
    }

    [[gnu::always_inline]]
    constexpr void IfPresent(Action<T> f)
    {
        if (_present)
            f(*_value);
    }

    [[gnu::always_inline]]
    constexpr T operator*()
    {
        if (!_present)
            Std::panic("No value present");

        return *_value;
    }

    [[gnu::always_inline]]
    constexpr T const& operator*() const
    {
        if (!_present)
            Std::panic("No value present");

        return *_value;
    }

    [[gnu::always_inline]]
    constexpr T Get(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        return *_value;
    }

    [[gnu::always_inline]]
    constexpr T OrElse(T other) const
    {
        return _present ? *_value : other;
    }

    [[gnu::always_inline]]
    constexpr T Take(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        T v = *_value;
        Clear();
        return v;
    }

    template <typename To>
    [[gnu::always_inline]]
    constexpr Optional<To> Select(Func<To(T)> f) const
    {
        if (_present)
            return f(*_value);
        return Optional<To>();
    }

    [[gnu::always_inline]]
    constexpr Optional<Std::RemoveRef<T>> Extract() const
    {
        if (_present)
            return *_value;
        return Empty();
    }

    template <typename... Args>
    [[gnu::always_inline]]
    constexpr auto operator()(Args&&... args) const
    {
        using OptRet = Optional<InvokeResult<T, Args...>>;

        if constexpr (SameAs<void, InvokeResult<T, Args...>>) {
            if (!_present) {
                return false;
            }
            (*_value)(Std::forward<Args>(args)...);
            return true;
        } else {
            if (!_present) {
                return OptRet();
            }
            return OptRet((*_value)(Std::forward<Args>(args)...));
        }
    }
};

template <typename T>
struct [[nodiscard]] Optional
{
    union
    {
        T     _value;
        Empty _empty;
    };
    bool _present;

    [[gnu::always_inline]]
    constexpr Optional()
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(Empty)
        : _empty()
        , _present(false)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(T const& value)
        : _value(value)
        , _present(true)
    {
    }

    template <typename U = T>
    [[gnu::always_inline]]
    constexpr Optional(U&& value)
        requires(!SameAs<Std::RemoveCvRef<U>, Optional<T>> &&
                 MoveConstructible<T, U>)
        : _value(Std::forward<U>(value))
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr Optional(Optional const& other)
        requires CopyConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(other._value);
    }

    [[gnu::always_inline]]
    constexpr Optional(Optional&& other)
        requires MoveConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(Std::move(other._value));
    }

    template <typename... Args>
    [[gnu::always_inline]] constexpr Optional(Args&&... args)
        : _value(Std::forward<Args>(args)...)
        , _present(true)
    {
    }

    [[gnu::always_inline]]
    constexpr ~Optional()
    {
        Clear();
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(Empty)
    {
        Clear();
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(T const& value)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::forward(value));
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(T&& value)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::move(value));
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(Optional const& other)
        requires CopyConstructible<T>
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                new (&_value) T(other._value);
        }
        return *this;
    }

    [[gnu::always_inline]]
    constexpr Optional& operator=(Optional&& other)
        requires MoveConstructible<T>
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                new (&_value) T(Std::move(other._value));
        }
        return *this;
    }

    [[gnu::always_inline]]
    void Clear()
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
    constexpr bool IsPresent() const
    {
        return _present;
    }

    [[gnu::always_inline]]
    constexpr void IfPresent(Action<T&> f)
    {
        if (_present)
            f(_value);
    }

    [[gnu::always_inline]]
    constexpr T& operator*()
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    // [[gnu::always_inline]]
    // constexpr T* operator->()
    // {
    //     if (!_present)
    //         Std::panic("No value present");

    //     return &_value;
    // }

    // [[gnu::always_inline]]
    // constexpr T const* operator->() const
    // {
    //     if (!_present)
    //         Std::panic("No value present");

    //     return &_value;
    // }

    [[gnu::always_inline]]
    constexpr T const& operator*() const
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    template <typename... Args>
    [[gnu::always_inline]]
    T& Emplace(Args&&... args)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::forward<Args>(args)...);
        return _value;
    }

    [[gnu::always_inline]]
    constexpr T& Get(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    [[gnu::always_inline]]
    constexpr T const& Get(const char* msg = "No value present") const
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    [[gnu::always_inline]]
    constexpr T OrElse(T other) const
    {
        return _present ? Std::move(_value) : other;
    }

    [[gnu::always_inline]]
    constexpr T Take(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        T v = Std::move(_value);
        Clear();
        return v;
    }

    template <typename To>
    [[gnu::always_inline]]
    constexpr Optional<To> Select(Func<To(T)> f) const
    {
        if (_present)
            return f(_value);
        return Optional<To>();
    }

    template <typename... Args>
    [[gnu::always_inline]]
    constexpr auto operator()(Args&&... args) const
    {
        using OptRet = Optional<InvokeResult<T, Args...>>;

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