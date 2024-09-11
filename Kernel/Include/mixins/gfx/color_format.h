#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>

template <typename T>
concept ColorFormat = requires(T col) {
    { T::BitLength } -> SameAs<u8>;
    { T() } -> SameAs<T>;
    { T(u8(0), u8(0), u8(0)) } -> SameAs<T>;
    { T(u32(0)) } -> SameAs<T>;
    { col.zero() } -> SameAs<T>;
    { col.composite(col) } -> SameAs<T>;
};

struct Rgb
{
    static constexpr u8 BitLength = 24;

    u8 _red, _green, _blue;

    [[gnu::always_inline]]
    constexpr Rgb() = default;

    [[gnu::always_inline]]
    constexpr Rgb(u8 red, u8 green, u8 blue)
        : _red(red)
        , _green(green)
        , _blue(blue)
    {
    }

    [[gnu::always_inline]]
    constexpr Rgb(u32 hex)
        : _red((hex >> 16) & 0xFF)
        , _green((hex >> 8) & 0xFF)
        , _blue(hex & 0xFF)
    {
    }
};

struct Rgba
{
    static constexpr u8 BitLength = 32;

    u8 _red, _green, _blue, _alpha;

    [[gnu::always_inline]]
    constexpr Rgba() = default;

    [[gnu::always_inline]]
    constexpr Rgba(u8 red, u8 green, u8 blue, u8 alpha = 0xFF)
        : _red(red)
        , _green(green)
        , _blue(blue)
        , _alpha(alpha)
    {
    }

    [[gnu::always_inline]]
    constexpr Rgba(u32 hex)
        : _red((hex >> 16) & 0xFF)
        , _green((hex >> 8) & 0xFF)
        , _blue(hex & 0xFF)
        , _alpha(0xFF)
    {
    }
};

struct Bgr
{
    static constexpr u8 BitLength = 24;

    u8 _blue, _green, _red;

    [[gnu::always_inline]]
    constexpr Bgr() = default;

    [[gnu::always_inline]]
    constexpr Bgr(u8 blue, u8 green, u8 red)
        : _blue(blue)
        , _green(green)
        , _red(red)
    {
    }

    [[gnu::always_inline]]
    constexpr Bgr(u32 hex)
        : _blue(hex & 0xFF)
        , _green((hex >> 8) & 0xFF)
        , _red((hex >> 16) & 0xFF)
    {
    }
};

struct Hsv
{
    static constexpr u8 BitLength = 24;

    u8 _hue, _saturation, _value;

    [[gnu::always_inline]]
    constexpr Hsv() = default;

    [[gnu::always_inline]]
    constexpr Hsv(u8 hue, u8 saturation, u8 value)
        : _hue(hue)
        , _saturation(saturation)
        , _value(value)
    {
    }
};

struct Hsl
{
    static constexpr u8 BitLength = 24;

    u8 _hue, _saturation, _lightness;

    [[gnu::always_inline]]
    constexpr Hsl() = default;

    [[gnu::always_inline]]
    constexpr Hsl(u8 hue, u8 saturation, u8 lightness)
        : _hue(hue)
        , _saturation(saturation)
        , _lightness(lightness)
    {
    }
};