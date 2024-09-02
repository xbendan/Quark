#pragma once

[[gnu::always_inline]]
constexpr auto
max(auto value)
{
    return value;
}

[[gnu::always_inline]]
constexpr auto
max(auto first, auto... rest)
{
    auto rhs = max(rest...);
    return first > rhs ? first : rhs;
}

[[gnu::always_inline]]
constexpr auto
min(auto value)
{
    return value;
}

[[gnu::always_inline]]
constexpr auto
min(auto first, auto... rest)
{
    auto rhs = min(rest...);
    return first < rhs ? first : rhs;
}

[[gnu::always_inline]]
constexpr auto
abs(auto value)
{
    return value < 0 ? -value : value;
}

[[gnu::always_inline]]
constexpr auto
clamp(auto value, auto min, auto max)
{
    return value<min ? min : value> max ? max : value;
}

[[gnu::always_inline]]
constexpr auto
lerp(auto a, auto b, auto t)
{
    return a + (b - a) * t;
}

[[gnu::always_inline]]
constexpr auto
lerp(auto a, auto b, auto t, auto min, auto max)
{
    return clamp(lerp(a, b, t), min, max);
}

[[gnu::always_inline]]
constexpr auto
lerp(auto a, auto b, auto t, auto min, auto max, auto minOut, auto maxOut)
{
    return lerp(lerp(a, b, t, min, max), minOut, maxOut);
}

[[gnu::always_inline]]
constexpr auto
invlerp(auto a, auto b, auto value)
{
    return (value - a) / (b - a);
}

[[gnu::always_inline]]
constexpr auto
pow(auto base, auto exp)
{
    auto result = 1;
    for (auto i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}