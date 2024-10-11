#pragma once

#include <mixins/meta/ref.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
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
        static void InitTasks(); // Initialize the scheduler

        static ProcessContext* CurrentContext();
        static void ScheduleToThread(ProcessContext* context, Thread* thread);
        static void ScheduleToNext(ProcessContext* context);
        static void ScheduleAllContexts();
        static u32  GetNextPID();

    private:
        Scheduler() = delete;

        static inline ArrayList<Process*> m_processes{};
        static inline u32                 m_nextPID{ 0 };
    };
} // namespace Quark::System::Task