#include <mixins/meta/tuple.h>
#include <mixins/std/utility.h>

namespace fmt {
    struct _Args
    {
        virtual ~_Args()            = default;
        virtual usize count() const = 0;
    };

    template <typename... Ts>
    struct Args : public _Args
    {
        Tuple<Std::RemoveCvRef<Ts>...> _data;

        Args(Ts&&... args)
            : _data{ Std::forward<Std::RemoveCvRef<Ts>>(args)... }
        {
        }
        ~Args() override = default;

        usize count() const override { return _data.count(); }
    };
}