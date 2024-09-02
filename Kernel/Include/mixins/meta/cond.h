#include <mixins/meta/error.h>
#include <mixins/meta/func.h>

template <typename T = Error>
inline constexpr void
assert(bool condition)
{
}

template <typename T = Error>
inline constexpr void
assert(bool condition, Func<T()> builder)
{
}