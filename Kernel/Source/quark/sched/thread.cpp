#include <quark/sched/thread.h>

namespace Quark::System::Task {
    Thread::Thread(Process* process, u32 threadId)
        : m_process(process)
        , m_state(State::Sleeping)
        , m_threadID(threadId)
        , m_priority(0)
    {
    }

    Thread::Thread(Process* process, State state, u32 threadId, u8 priority)
        : m_process(process)
        , m_state(state)
        , m_threadID(threadId)
        , m_priority(priority)
    {
    }
}