#include <mixins/std/c++types.h>

namespace Qk {
    enum struct Align
    {
        LEFT,
        RIGHT,
        CENTER
    };

    template <typename T>
    struct Aligned
    {
        T     _inner;
        Align _align;
        usize _width;
    };

    template <typename T>
    Aligned<T> align(T inner, Align align, usize width)
    {
        return { inner, align, width };
    }
}
