#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/utility.h>

template <typename... Ts>
struct Tuple;

template <class T>
struct Tuple<T>
{
    T data;

    template <usize I>
    auto& get()
    {
        return data;
    }

    constexpr usize count() const { return 1; }

    auto& operator[](usize i) { return data; }
};

template <class T, class... Ts>
    requires(sizeof...(Ts) > 0)
struct Tuple<T, Ts...>
{
    T            data;
    Tuple<Ts...> next;

    template <usize I>
    auto& get()
    {
        if constexpr (I == 0)
            return data;
        else
            return next.template get<I - 1>();
    }

    constexpr usize count() const { return 1 + next.count(); }

    auto& operator[](usize i)
    {
        if (i == 0)
            return data;
        else
            return next[i - 1];
    }
};