#include <mixins/std/concepts.h>
#include <mixins/utils/date.h>

namespace Quark::System::System {

    // template <typename T>
    // concept ClockSource = requires(T& t) {
    //     { t.getTicks() } -> ConvertibleTo<u64>;
    //     { t.now() } -> SameAs<Std::Date>;
    // };

    class IClockSource
    {
    public:
        virtual u64  getTicks()            = 0;
        virtual Date now()                 = 0;
        virtual void sleep(u64 ms)         = 0;
        virtual void sleepUntil(Date date) = 0;
    };
}