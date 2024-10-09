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
        static void schedule(ProcessContext* context, Thread* thread);
        static void schedule(ProcessContext* context);
        static void scheduleAll();
        static u32  GetNextPID();

    private:
        Scheduler() = delete;

        static inline u32 m_nextPID{ 0 };
    };
} // namespace Quark::System::Task