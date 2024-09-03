#include <mixins/meta/result.h>

namespace Quark::System::Hal {
    class InterruptsController
    {
    public:
        Res<> setHandler(u8 intno, void (*handler)(void*));
    };
}