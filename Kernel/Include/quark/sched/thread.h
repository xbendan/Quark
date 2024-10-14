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

        Thread() = delete;
        Thread(Process* process, u32 threadId);
        Thread(Process* process, State state, u32 threadId, u8 priority);
        ~Thread() = default;

        /* --- Methods --- */
        Process const* GetProcess() const { return m_process; }
        State          GetState() const { return m_state; }
        u32            GetThreadID() const { return m_threadID; }
        u8             GetPriority() const { return m_priority; }

    private:
        Process const* m_process;
        State          m_state;
        u32            m_threadID;
        u8             m_priority;
    };
} // namespace Quark::System::Task