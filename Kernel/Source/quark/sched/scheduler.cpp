#include <mixins/utils/linked_queue.h>
#include <quark/api/logging.h>
#include <quark/hal/multiprocessing.h>
#include <quark/os/main.h>

namespace Quark::System::Task {
    IReadOnlyCollection<Hal::ICPULocal*>* _cpus;
    Array<IQueue<Thread*>*>               _threadQueues;

    void Process::addProcess(Process* process) {}

    void Process::addProcess(Process* process, u16 id) {}

    void Process::destroyProcess(Process* process) {}

    void Process::destroyProcess(u16 id) {}

    Process* Process::getProcessById(u16 id)
    {
        return nullptr;
    }

    void schedule(ProcessContext* context, Thread* thread) {}

    void schedule(ProcessContext* context) {}

    void scheduleAll() {}
}

namespace Quark::System {
    using namespace Quark::System::Task;

    Res<> initTasks()
    {
        _cpus = Hal::setupMultiprocessing().unwrap();

        static_cast<ArrayList<Hal::ICPULocal*>*>(_cpus)->forEachOrdered(
            [](Hal::ICPULocal* const& cpu, usize i) {
                _threadQueues[i] = new LinkedQueue<Thread*>();
            });

        return Ok();
    }
} // namespace Quark::System