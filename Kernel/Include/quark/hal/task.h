#pragma once

#include <mixins/std/c++types.h>

#include <quark/sched/thread.h>

namespace Quark::Hal {
    using Quark::Task::Thread;

    struct CPULocal
    {
        u32           _id;
        Thread*       _currentThread;
        Thread* const _idleThread;
    };
}