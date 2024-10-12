#include <quark/sched/pid_namespace.h>

namespace Quark::System::Task {
    PidNamespace::PidNamespace()
        : m_nextId(2048)
        , m_pidmap(32768)
    {
    }

    u32 PidNamespace::NextPid()
    {
        u32 pid = m_nextId.fetchInc();

        if (pid >= m_pidmap.Size()) {
            pid = 2048;
            m_nextId.store(pid);
        }

        if (m_pidmap.at(pid)) {
            pid = m_pidmap.find(1);
            m_nextId.store(pid);
        }

        return pid;
    }
}