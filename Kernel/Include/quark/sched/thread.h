#pragma once

#include <mixins/std/c++types.h>

namespace Quark::System::Task {
    class Process;

    class Thread
    {
    public:
        enum class State
        {
            Running,
            Waiting,
            Sleeping,
            Terminated,
            Zombie,
        };

        /* --- Constructors --- */

        Thread() = default;
        Thread(Process* process, u32 threadId);
        ~Thread() = default;

        /* --- Methods --- */
        Process const* _process;
        State          _state;
        u32            _threadId;
        u8             _priority;
    };
} // namespace Quark::System::Task