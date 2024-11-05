#pragma once

#include <mixins/meta/ref.h>
#include <mixins/std/c++types.h>
#include <quark/hal/multiprocessing.h>
#include <quark/sched/pid_namespace.h>
#include <quark/sched/process.h>

namespace Quark::System::Task {

    struct ProcessContext
    {
        u32     _id;
        Thread* _currentThread;
        Thread* _idleThread;
    };

    class Scheduler
    {
    public:
        static Res<> InitTasks(); // Initialize the scheduler

        static ProcessContext* CurrentContext();
        static void ScheduleToThread(ProcessContext* context, Thread* thread);
        static void ScheduleToNext(ProcessContext* context);
        static void ScheduleAllContexts();
        static PidNamespace& GetDefaultPidNamespace();

    private:
        Scheduler() = delete;

        static inline PidNamespace                 m_pidNamespace{};
        static inline List<Hal::ICPULocalDevice*>* m_devices;
        static inline u32                          m_nextPID{ 0 };
    };
} // namespace Quark::System::Task