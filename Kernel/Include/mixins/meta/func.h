#pragma once

#include <mixins/std/concepts.h>
#include <mixins/std/type_traits.h>
#include <mixins/std/utility.h>

template <typename>
class Func;

template <typename Ret, typename... Args>
class Func<Ret(Args...)>
{
public:
    Func() = delete;

    template <Callable<Args...> Call>
        requires(Std::isConvertible<Call, Ret(Args...)>)
    Func(Call&& call)
        : _isWrapped(false)
    {
        _func = reinterpret_cast<Ret (*)(Args...)>(call);
    }

    template <Callable<Args...> Call>
        requires(Std::isConvertible<Call, Ret(Args...)>) &&
                (sizeof(Call) <= sizeof(Ret(*)(Args...)))
    Func(Call&& call)
        : _isWrapped(false)
    {
        _func = reinterpret_cast<Ret (*)(Args...)>(call);
    }

    template <Callable<Args...> Call>
        requires(!Std::isConvertible<Call, Ret(Args...)>) ||
                (sizeof(Call) > sizeof(Ret(*)(Args...)))
    Func(Call&& call)
        : _isWrapped(true)
    {
        _wrap = new FuncWrapImpl<Call>(Std::move(call));
    }

    Func(Ret (*func)(Args...))
        : _isWrapped(false)
    {
        _func = func;
    }

    Func(Func<Ret(Args...)> const& other)
        : _isWrapped(other._isWrapped)
    {
        if (_isWrapped) {
            _wrap = new FuncWrapImpl<Args...>(*other._wrap);
        } else {
            _func = other._func;
        }
    }

    Func(Func<Ret(Args...)>&& other)
        : _isWrapped(other._isWrapped)
    {
        if (_isWrapped) {
            _wrap = other._wrap;
        } else {
            _func = other._func;
        }
        other._isWrapped = false;
    }

    ~Func()
    {
        if (_isWrapped) {
            delete _wrap;
        }
    }

    bool isWrapped() const { return _isWrapped; }

    bool operator==(Func const& other) const
    {
        if (_isWrapped != other._isWrapped) {
            return false;
        }
        if (_isWrapped) {
            return _wrap == other._wrap;
        }
        return _func == other._func;
    }

    bool operator!=(Func const& other) const { return !(*this == other); }

    Ret operator()(Args... args)
    {
        if (_isWrapped) {
            return (*_wrap)(args...);
        }
        return _func(args...);
    }

    template <Callable<Args...> _Callable>
        requires(Std::isConvertible<_Callable, Ret(Args...)>)
    Func& operator=(_Callable&& call)
    {
        if (_isWrapped) {
            delete _wrap;
        }
        _isWrapped = false;
        _func      = reinterpret_cast<Ret (*)(Args...)>(call);
        return *this;
    }

    template <Callable<Args...> _Callable>
        requires(!Std::isConvertible<_Callable, Ret(Args...)>)
    Func& operator=(_Callable&& call)
    {
        if (_isWrapped) {
            delete _wrap;
        }
        _isWrapped = true;
        _wrap      = new FuncWrapImpl<_Callable>(Std::move(call));
        return *this;
    }

    Func& operator=(Func const& other)
    {
        if (_isWrapped) {
            delete _wrap;
        }
        _isWrapped = other._isWrapped;
        if (_isWrapped) {
            _wrap = new FuncWrapImpl<Args...>(*other._wrap);
        } else {
            _func = other._func;
        }
        return *this;
    }

    Func& operator=(Func&& other)
    {
        if (_isWrapped) {
            delete _wrap;
        }

        _isWrapped = other._isWrapped;
        if (_isWrapped) {
            _wrap = other._wrap;
        } else {
            _func = other._func;
        }
        other._isWrapped = false;
        return *this;
    }

private:
    struct FuncWrap
    {
        virtual Ret operator()(Args...) = 0;
        virtual ~FuncWrap()             = default;
    };

    template <Callable<Args...> _Callable>
    struct FuncWrapImpl : FuncWrap
    {
        FuncWrapImpl(_Callable&& call)
            : _call(Std::move(call))
        {
        }
        Ret operator()(Args... args) override { return _call(args...); }

        _Callable _call;
    };

    bool _isWrapped;
    union
    {
        Ret (*_func)(Args...);
        FuncWrap* _wrap;
    };
};

using Runnable = Func<void()>;

template <typename T>
using Predicate = Func<bool(T)>;

template <typename T>
using Action = Func<void(T)>;

template <typename T>
using Supplier = Func<T()>;

template <typename T>
using Comparator = Func<int(T, T)>;

template <typename T>
using BinaryOperator = Func<T(T, T)>;