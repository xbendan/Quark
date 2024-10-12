#include <mixins/concurrent/atomic.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/bitmap.h>

namespace Quark::System::Task {
    struct Pid
    {};

    class PidList
    {
        Atomic<int> m_count;
    };
}