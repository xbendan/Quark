#include <mixins/utils/linked_queue.h>
#include <quark/api/logging.h>
#include <quark/hal/multiprocessing.h>
#include <quark/os/main.h>

namespace Quark::System::Task {
    ICollection<Hal::ICPULocalDevice*>* _cpus;
    Array<IQueue<Thread*>*>             _threadQueues;
}

namespace Quark::System {
    using namespace Quark::System::Task;

    Res<> InitTasks()
    {
        _cpus = Hal::setupMultiprocessing().Unwrap();
        static_cast<IList<Hal::ICPULocalDevice*>*>(_cpus)->ForEachOrdered(
            [](Hal::ICPULocalDevice* const& cpu, usize i) {
                _threadQueues[i] = new LinkedQueue<Thread*>();
                cpu->SendSignal(Hal::Signal::SCHED);
            });

        return Ok();
    }
} // namespace Quark::System