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

    void schedule(ProcessContext* context, Thread* thread);
    void schedule(ProcessContext* context);
    void scheduleAll();
    u32  nextPID();
} // namespace Quark::System::Task