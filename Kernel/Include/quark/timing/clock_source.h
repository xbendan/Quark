#include <mixins/utils/singleton.h>

namespace Quark::System {
    using Qk::Singleton;

    class ClockSource
    {
    public:
        virtual Date GetToday()     = 0;
        virtual u64  GetTimestamp() = 0;

        static inline Singleton<ClockSource*> Global;
    };
}