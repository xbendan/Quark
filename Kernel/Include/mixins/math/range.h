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

    __attribute__((always_inline)) constexpr inline Range(Range&& other)
        : _min(other._min)
        , _max(other._max)
    {
    }

    __attribute__((always_inline)) constexpr inline Range(Range const& other)
        : _min(other._min)
        , _max(other._max)
    {
    }

    [[gnu::always_inline]]
    T Clamp(T value) const
    {
        return value < _min ? _min : value > _max ? _max : value;
    }

    [[gnu::always_inline]]
    T Length() const
    {
        return _max - _min;
    }

    [[gnu::always_inline]]
    T Lerp(T t) const
    {
        return _min + t * Range();
    }

    [[gnu::always_inline]]
    T InvLerp(T value) const
    {
        return (value - _min) / Range();
    }

    [[gnu::always_inline]]
    T Remap(T value, const Range& other) const
    {
        return other.Lerp(InvLerp(value));
    }

    [[gnu::always_inline]]
    T Remap(T value, T min, T max) const
    {
        return Lerp((value - min) / (max - min));
    }

    [[gnu::always_inline]]
    T Remap(T value, T min, T max, T newMin, T newMax) const
    {
        return newMin + (newMax - newMin) * (value - min) / (max - min);
    }

    [[gnu::always_inline]]
    T Remap(T value, const Range& other, T newMin, T newMax) const
    {
        return newMin + (newMax - newMin) * InvLerp(value);
    }

    [[gnu::always_inline]]
    T Remap(T value, T min, T max, const Range& newRange) const
    {
        return newRange.Lerp((value - min) / (max - min));
    }

    [[gnu::always_inline]]
    bool WithinRange(T value) const
    {
        return value >= _min && value <= _max;
    }

    T ConstraintsTo(T value) const
    {
        return value < _min ? _min : value > _max ? _max : value;
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