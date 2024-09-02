#pragma once

#include <mixins/std/type_traits.h>

template <typename T = u64>
    requires Std::isArithmetic<T>
struct Range
{
    T _min;
    T _max;

    [[gnu::always_inline]]
    constexpr Range()
        : _min(0)
        , _max(0)
    {
    }

    [[gnu::always_inline]]
    constexpr Range(T min, T max)
        : _min(min)
        , _max(max)
    {
    }

    [[gnu::always_inline]]
    T clamp(T value) const
    {
        return value<_min ? _min : value> _max ? _max : value;
    }

    [[gnu::always_inline]]
    T range() const
    {
        return _max - _min;
    }

    [[gnu::always_inline]]
    T lerp(T t) const
    {
        return _min + t * range();
    }

    [[gnu::always_inline]]
    T invlerp(T value) const
    {
        return (value - _min) / range();
    }

    [[gnu::always_inline]]
    T remap(T value, const Range& other) const
    {
        return other.lerp(invlerp(value));
    }

    [[gnu::always_inline]]
    T remap(T value, T min, T max) const
    {
        return lerp((value - min) / (max - min));
    }

    [[gnu::always_inline]]
    T remap(T value, T min, T max, T newMin, T newMax) const
    {
        return newMin + (newMax - newMin) * (value - min) / (max - min);
    }

    [[gnu::always_inline]]
    T remap(T value, const Range& other, T newMin, T newMax) const
    {
        return newMin + (newMax - newMin) * invlerp(value);
    }

    [[gnu::always_inline]]
    T remap(T value, T min, T max, const Range& newRange) const
    {
        return newRange.lerp((value - min) / (max - min));
    }

    [[gnu::always_inline]]
    bool contains(T value) const
    {
        return value >= _min && value <= _max;
    }

    T constrainTo(T value) const
    {
        return value<_min ? _min : value> _max ? _max : value;
    }

    [[gnu::always_inline]]
    bool operator==(const Range& other) const
    {
        return _min == other._min && _max == other._max;
    }

    [[gnu::always_inline]]
    bool operator!=(const Range& other) const
    {
        return !(*this == other);
    }

    [[gnu::always_inline]]
    bool operator<(const Range& other) const
    {
        return _min < other._min && _max < other._max;
    }

    [[gnu::always_inline]]
    bool operator>(const Range& other) const
    {
        return _min > other._min && _max > other._max;
    }

    [[gnu::always_inline]]
    bool operator<=(const Range& other) const
    {
        return _min <= other._min && _max <= other._max;
    }

    [[gnu::always_inline]]
    bool operator>=(const Range& other) const
    {
        return _min >= other._min && _max >= other._max;
    }

    [[gnu::always_inline]]
    Range& operator+=(T value)
    {
        _min += value;
        _max += value;
        return *this;
    }

    [[gnu::always_inline]]
    Range& operator-=(T value)
    {
        _min -= value;
        _max -= value;
        return *this;
    }

    [[gnu::always_inline]]
    Range& operator*=(T value)
    {
        _min *= value;
        _max *= value;
        return *this;
    }

    [[gnu::always_inline]]
    Range& operator/=(T value)
    {
        _min /= value;
        _max /= value;
        return *this;
    }

    [[gnu::always_inline]]
    Range operator+(T value) const
    {
        return { _min + value, _max + value };
    }

    [[gnu::always_inline]]
    Range operator-(T value) const
    {
        return { _min - value, _max - value };
    }

    [[gnu::always_inline]]
    Range operator*(T value) const
    {
        return { _min * value, _max * value };
    }

    [[gnu::always_inline]]
    Range operator/(T value) const
    {
        return { _min / value, _max / value };
    }

    [[gnu::always_inline]]
    Range operator-() const
    {
        return { -_max, -_min };
    }
};