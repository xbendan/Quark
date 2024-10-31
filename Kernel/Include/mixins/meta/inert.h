#pragma once

#include <mixins/std/utility.h>

template <typename T>
struct Inert
{
    alignas(alignof(T)) char _data[sizeof(T)];

    template <typename... Args>
    always_inline T* ctor(Args&&... args)
    {
        return new (&unwrap()) T(Std::forward<Args>(args)...);
    }

    always_inline void dtor() { unwrap().~T(); }

    always_inline T& unwrap() { return *(T*)_data; }

    always_inline const T& unwrap() const { return *(T*)_data; }

    always_inline T take()
    {
        T val = Std::move(unwrap());
        unwrap().~T();
        return val;
    }

    template <typename... Args>
    always_inline T* operator()(Args&&... args)
    {
        return new (&unwrap()) T(Std::forward<Args>(args)...);
    }
};