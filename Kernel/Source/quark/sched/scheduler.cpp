#include <mixins/utils/linked_queue.h>
#include <quark/api/logging.h>
#include <quark/hal/multiprocessing.h>
#include <quark/sched/sched.h>
#include <quark/sched/thread.h>

namespace Quark::System::Task {
    ICollection<Hal::ICPULocalDevice*>* _cpus;
    Array<IQueue<Thread*>*>             _threadQueues;

    Res<> Scheduler::InitTasks()
    {
        new (&m_pidNamespace) PidNamespace();
        new (&m_processes) ArrayList<Process*>();
        auto devices = Hal::SetupMultiprocessing();
        if (devices.IsError()) {
            return Error::SystemError("Failed to setup multiprocessing");
        }

        (m_devices = devices.Unwrap())
            ->ForEachOrdered([](Hal::ICPULocalDevice* const& cpu, usize i) {
                cpu->_threadQueue = new LinkedQueue<Thread*>();
                cpu->SendSignal(Hal::Signal::SCHED);
            });
        return Ok();
    }

    PidNamespace& Scheduler::GetDefaultPidNamespace()
    {
        return m_pidNamespace;
    }
}

namespace Quark::System {
    using namespace Quark::System::Task;

    Res<> InitTasks()
    {
        _cpus = Hal::SetupMultiprocessing().Unwrap();
        static_cast<IList<Hal::ICPULocalDevice*>*>(_cpus)->ForEachOrdered(
            [](Hal::ICPULocalDevice* const& cpu, usize i) {
                _threadQueues[i] = new LinkedQueue<Thread*>();
                cpu->SendSignal(Hal::Signal::SCHED);
            });

        return Ok();
    }
} // namespace Quark::System