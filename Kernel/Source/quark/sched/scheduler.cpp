#include <mixins/utils/linked_queue.h>
#include <quark/api/logging.h>
#include <quark/hal/multiprocessing.h>
#include <quark/os/main.h>

namespace Quark::System::Task {
    IReadOnlyCollection<Hal::ICPULocalDevice*>* _cpus;
    Array<IQueue<Thread*>*>                     _threadQueues;

    void Process::addProcess(Process* process) {}

    void Process::addProcess(Process* process, u16 id) {}

    void Process::destroyProcess(Process* process) {}

    void Process::destroyProcess(u16 id) {}

    Process* Process::getProcessById(u16 id)
    {
        return nullptr;
    }
}

namespace Quark::System {
    using namespace Quark::System::Task;

    Res<> InitTasks()
    {
        _cpus = Hal::setupMultiprocessing().unwrap();
        static_cast<IReadOnlyList<Hal::ICPULocalDevice*>*>(_cpus)
            ->ForEachOrdered([](Hal::ICPULocalDevice* const& cpu, usize i) {
                _threadQueues[i] = new LinkedQueue<Thread*>();
                cpu->sendSignal(Hal::Signal::SCHED);
            });

        return Ok();
    }
} // namespace Quark::System