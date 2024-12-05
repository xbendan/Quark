#include <mixins/utils/queue.h>
#include <mixins/utils/requires/constraint.h>
#include <quark/hal/multiprocessing.h>
#include <quark/sched/sched.h>
#include <quark/sched/thread.h>

namespace Quark::System::Task {
    using Qk::Array;
    using Qk::Constraint;
    using Qk::List;
    using Qk::Queue;

    Queue<Thread*> s_threadQueues[MAX_PRIORITY];
    Bitmap*        s_bitmap;

    void ScheduleYield();

    void SchedulePreempt();

    u8 SetThreadPriority(Thread* thread, Constraint<u8, 32> newPriority);

    u8 GetThreadPriority(Thread* thread);

    int GetPriorityTimeSlice(u8 priority)
    {
        int slice = 100;

        slice *= 1 << (priority / 6);
        slice /= 8;

        return slice;
    }

    void BalanceRunQueues() {}

    List<Hal::ICPULocalDevice*>* _cpus;
    Array<Queue<Thread*>*>       _threadQueues;

    Res<> Scheduler::InitTasks()
    {
        new (&m_pidNamespace) PidNamespace();
        auto devices = Hal::SetupMultiprocessing();
        if (devices.isError()) {
            return Error::SystemError("Failed to setup multiprocessing");
        }

        (m_devices = devices.unwrap())
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
        _cpus = Hal::SetupMultiprocessing().unwrap();
        _cpus->ForEachOrdered([](Hal::ICPULocalDevice* const& cpu, usize i) {
            _threadQueues[i] = new Queue<Thread*>();
            cpu->SendSignal(Hal::Signal::SCHED);
        });

        return Ok();
    }
} // namespace Quark::System