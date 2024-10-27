#pragma once

#include <mixins/std/concepts.h>
#include <mixins/std/type_traits.h>

namespace Math {

}

template <typename T, typename U = T>
    requires Std::isArithmetic<T> && Std::isArithmetic<U>
T
DivCeil(T a, U b)
{
    return (a / b) + (a % b != 0);
}

template <typename T, typename U = T>
    requires Std::isArithmetic<T> && Std::isArithmetic<U>
T
DivFloor(T a, U b)
{
    return a / b;
}

template <typename T>
    requires(Std::isArithmetic<T> && Comparable<T>)
int
Compare(T a, T b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}