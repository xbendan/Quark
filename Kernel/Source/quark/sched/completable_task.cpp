#include <quark/sched/completable.h>
#include <quark/timing/timer_source.h>

namespace Quark::System::Task {
    void Delay(u32 ms)
    {
        TimerSource::Global()->Sleep(ms);
    }

    void Delay(TimeSpan span) {}

}