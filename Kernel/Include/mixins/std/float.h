#include <mixins/std/c++types.h>

template <usize S, usize E, usize M>
    requires((S <= 1) && (E >= 1) && (M >= 1))
struct BitField
{
    constexpr static usize Size = S + E + M;
    static_assert(((Size - 1) | (Size)) == Size * 2 - 1);

    constexpr static usize Sign     = S;
    constexpr static usize Exponent = E;
    constexpr static usize Mantissa = M;

    struct
    {
        u64 mantissa : M;
        u64 exponent : E;
        u64 sign : S;
    } __attribute__((packed));
};

template <typename T>
union Float;

template <>
union Float<f32>
{
    f32                value;
    BitField<1, 8, 23> bits;
};

template <>
union Float<f64>
{
    f64                 value;
    BitField<1, 11, 52> bits;
};

// template <typename T>
// union Float<f128>
// {
//     f128                 value;
//     BitField<1, 15, 112> bits;
// };