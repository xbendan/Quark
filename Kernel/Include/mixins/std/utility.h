#pragma once

#include <mixins/std/type_traits.h>

// template <typename T>
// typename Std::AddRValueReference<T>
// declval() noexcept
// {
//     return static_cast<typename Std::AddRValueReference<T>>(*((T*)nullptr));
// }

namespace Std {
    template <typename T>
    T&& forward(typename Std::RemoveRef<T>& p) noexcept
    {
        return static_cast<T&&>(p);
    }

    template <typename T>
    T&& forward(typename Std::RemoveRef<T>&& p) noexcept
    {
        return static_cast<T&&>(p);
    }

    template <typename T>
    constexpr typename Std::RemoveRef<T>&& move(T&& p) noexcept
    {
        return static_cast<typename Std::RemoveRef<T>&&>(p);
    }

    template <typename TOld, typename TNew = TOld>
    constexpr inline TOld exchange(TOld& obj, TNew&& new_value)
    {
        TOld old_value = Std::move(obj);
        obj            = Std::forward<TNew>(new_value);
        return old_value;
    }
}