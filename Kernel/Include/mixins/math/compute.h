#include <mixins/std/type_traits.h>

template <typename T, typename U = T>
    requires Std::isArithmetic<T> && Std::isArithmetic<U>
T
divCeil(T a, U b)
{
    return (a / b) + (a % b != 0);
}

template <typename T, typename U = T>
    requires Std::isArithmetic<T> && Std::isArithmetic<U>
T
divFloor(T a, U b)
{
    return a / b;
}