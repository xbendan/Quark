#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/union.h>
#include <mixins/std/concepts.h>
#include <mixins/std/panic.h>
#include <mixins/std/utility.h>

template <typename T>
struct Opt;

template <typename T>
    requires Std::isReference<T>
struct [[nodiscard]] Opt<T>
{
    using TPureType  = Std::RemoveCvRef<T>;
    using TInnerType = Std::AddPointer<TPureType>;

    union
    {
        TInnerType _value;
        Empty      _empty;
    };
    bool _present;

    always_inline constexpr Opt()
        : _empty()
        , _present(false)
    {
    }

    always_inline constexpr Opt(Empty)
        : _empty()
        , _present(false)
    {
    }

    always_inline constexpr Opt(TInnerType value)
        : _value(value)
        , _present(true)
    {
    }

    always_inline constexpr Opt(T value)
        : _value(&value)
        , _present(true)
    {
    }

    always_inline constexpr Opt(Opt const& other)
        : _present(other._present)
    {
        if (_present)
            _value = other._value;
    }

    always_inline constexpr Opt(Opt&& other)
        : _present(other._present)
    {
        if (_present)
            _value = other._value;
    }

    always_inline constexpr ~Opt() { Clear(); }

    always_inline constexpr Opt& operator=(Empty)
    {
        Clear();
        return *this;
    }

    always_inline constexpr Opt& operator=(TInnerType value)
    {
        Clear();

        _present = true;
        _value   = value;
        return *this;
    }

    always_inline constexpr Opt& operator=(T value)
    {
        Clear();

        _present = true;
        _value   = &value;
        return *this;
    }

    always_inline

        constexpr Opt&
        operator=(Opt const& other)
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                _value = other._value;
        }
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt&& other)
    {
        if (this != &other) {
            Clear();

            _present = other._present;
            if (_present)
                _value = other._value;
        }
        return *this;
    }

    always_inline void Clear()
    {
        if (_present) {
            _value   = nullptr;
            _present = false;
        }
    }

    always_inline constexpr explicit operator bool() const { return _present; }

    always_inline constexpr bool IsPresent() const { return _present; }

    always_inline constexpr void IfPresent(Action<T> f)
    {
        if (_present)
            f(*_value);
    }

    always_inline constexpr T operator*()
    {
        if (!_present)
            Std::panic("No value present");

        return *_value;
    }

    always_inline constexpr T const& operator*() const
    {
        if (!_present)
            Std::panic("No value present");

        return *_value;
    }

    always_inline constexpr T Get(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        return *_value;
    }

    always_inline constexpr T OrElse(T other) const
    {
        return _present ? *_value : other;
    }

    always_inline constexpr T Take(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        T v = *_value;
        Clear();
        return v;
    }

    template <typename To>
    always_inline constexpr Opt<To> Select(Func<To(T)> f) const
    {
        if (_present)
            return f(*_value);
        return Empty();
    }

    always_inline constexpr Opt<Std::RemoveRef<T>> Extract() const
    {
        if (_present)
            return *_value;
        return Empty();
    }

    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) const
    {
        using OptRet = Opt<InvokeResult<T, Args...>>;

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
struct [[nodiscard]] Opt
{
    union
    {
        T     _value;
        Empty _empty;
    };
    bool _present;

    always_inline constexpr Opt()
        : _empty()
        , _present(false)
    {
    }

    always_inline constexpr Opt(Empty)
        : _empty()
        , _present(false)
    {
    }

    always_inline constexpr Opt(T const& value)
        : _value(value)
        , _present(true)
    {
    }

    template <typename U = T>
    always_inline constexpr Opt(U&& value)
        requires(!SameAs<Std::RemoveCvRef<U>, Opt<T>> &&
                 MoveConstructible<T, U>)
        : _value(Std::forward<U>(value))
        , _present(true)
    {
    }

    always_inline constexpr Opt(Opt const& other)
        requires CopyConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(other._value);
    }

    always_inline constexpr Opt(Opt&& other)
        requires MoveConstructible<T>
        : _present(other._present)
    {
        if (_present)
            new (&_value) T(Std::move(other._value));
    }

    template <typename... Args>
    always_inline constexpr Opt(Args&&... args)
        : _value(Std::forward<Args>(args)...)
        , _present(true)
    {
    }

    always_inline constexpr ~Opt() { Clear(); }

    always_inline constexpr Opt& operator=(Empty)
    {
        Clear();
        return *this;
    }

    always_inline constexpr Opt& operator=(T const& value)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::forward(value));
        return *this;
    }

    always_inline constexpr Opt& operator=(T&& value)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::move(value));
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt const& other)
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

    always_inline constexpr Opt& operator=(Opt&& other)
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

    always_inline constexpr bool operator==(Empty) const { return !_present; }

    template <typename U>
        requires Equatable<T, U>
    always_inline constexpr bool operator==(U const& other) const
    {
        if (!_present)
            return false;

        return _value == other;
    }

    always_inline constexpr void Clear()
    {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    always_inline constexpr explicit operator bool() const { return _present; }

    always_inline constexpr bool IsPresent() const { return _present; }

    always_inline constexpr void IfPresent(Action<T&> f)
    {
        if (_present)
            f(_value);
    }

    always_inline constexpr T& operator*()
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    // always_inline
    // constexpr T* operator->()
    // {
    //     if (!_present)
    //         Std::panic("No value present");

    //     return &_value;
    // }

    // always_inline
    // constexpr T const* operator->() const
    // {
    //     if (!_present)
    //         Std::panic("No value present");

    //     return &_value;
    // }

    always_inline constexpr T const& operator*() const
    {
        if (!_present)
            Std::panic("No value present");

        return _value;
    }

    template <typename... Args>
    always_inline T& Emplace(Args&&... args)
    {
        Clear();

        _present = true;
        new (&_value) T(Std::forward<Args>(args)...);
        return _value;
    }

    always_inline constexpr T& Get(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    always_inline constexpr T const& Get(
        const char* msg = "No value present") const
    {
        if (!_present)
            Std::panic(msg);

        return _value;
    }

    always_inline constexpr T OrElse(T other) const
    {
        return _present ? Std::move(_value) : other;
    }

    always_inline constexpr T Take(const char* msg = "No value present")
    {
        if (!_present)
            Std::panic(msg);

        T v = Std::move(_value);
        Clear();
        return v;
    }

    template <typename To>
    always_inline constexpr Opt<To> Select(Func<To(T)> f) const
    {
        if (_present)
            return f(_value);
        return Opt<To>();
    }

    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) const
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