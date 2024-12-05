#include <mixins/std/assert.h>
#include <mixins/std/concepts.h>

namespace Qk {
    template <Integral T, T Tmax>
    struct Constraint
    {
        static constexpr T max = Tmax;

        constexpr Constraint() = default;
        constexpr Constraint(T value)
            : _value(value)
        {
            assert(value <= max);
        }

        T _value;
    };
}