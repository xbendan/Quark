#include <quark/os/main.h>

namespace Quark::System {
    namespace Task {
        Scheduler scheduler;

        void Scheduler::addProcess(Process* process)
        {
            m_processes[process->_processId] = RefPtr(process);
        }
    };

    Res<> initTasks()
    {
        new (&Task::scheduler) Task::Scheduler();

        return Ok();
    }

    namespace API {
        Scheduler* getScheduler()
        {
            return &Task::scheduler;
        }
    }
}