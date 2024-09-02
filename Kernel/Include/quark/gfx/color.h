#include <mixins/std/c++types.h>

struct Rgba
{
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