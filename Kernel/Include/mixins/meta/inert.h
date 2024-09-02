#include <mixins/std/utility.h>

template <typename T>
struct Inert
{
    alignas(alignof(T)) char _data[sizeof(T)];

    template <typename... Args>
    [[gnu::always_inline]]
    T* ctor(Args&&... args)
    {
        return new (&unwrap()) T(Std::forward<Args>(args)...);
    }

    [[gnu::always_inline]]
    void dtor()
    {
        unwrap().~T();
    }

    [[gnu::always_inline]]
    T& unwrap()
    {
        return *(T*)_data;
    }

    [[gnu::always_inline]]
    const T& unwrap() const
    {
        return *(T*)_data;
    }

    [[gnu::always_inline]]
    T take()
    {
        T val = Std::move(unwrap());
        unwrap().~T();
        return val;
    }

    template <typename... Args>
    [[gnu::always_inline]]
    T* operator()(Args&&... args)
    {
        return new (&unwrap()) T(Std::forward<Args>(args)...);
    }
};