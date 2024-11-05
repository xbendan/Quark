#include <mixins/utils/queue.h>
#include <quark/hal/multiprocessing.h>
#include <quark/sched/sched.h>
#include <quark/sched/thread.h>

namespace Quark::System::Task {
    using Qk::Array;
    using Qk::List;
    using Qk::Queue;

    List<Hal::ICPULocalDevice*>* _cpus;
    Array<Queue<Thread*>*>       _threadQueues;

    Res<> Scheduler::InitTasks()
    {
        new (&m_pidNamespace) PidNamespace();
        auto devices = Hal::SetupMultiprocessing();
        if (devices.IsError()) {
            return Error::SystemError("Failed to setup multiprocessing");
        }

        (m_devices = devices.Unwrap())
            ->ForEachOrdered([](Hal::ICPULocalDevice* const& cpu, usize i) {
                cpu->_threadQueue = new Queue<Thread*>();
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
        _cpus->ForEachOrdered([](Hal::ICPULocalDevice* const& cpu, usize i) {
            _threadQueues[i] = new Queue<Thread*>();
            cpu->SendSignal(Hal::Signal::SCHED);
        });

        return Ok();
    }
} // namespace Quark::System