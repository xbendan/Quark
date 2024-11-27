#include <mixins/meta/tuple.h>

namespace Qk {

    template <typename... TArgs>
    struct Exception
    {
        const char*     _message;
        Tuple<TArgs...> _args;

        always_inline constexpr Tuple<TArgs...>* operator->() const
        {
            return &_args;
        }
    };
}