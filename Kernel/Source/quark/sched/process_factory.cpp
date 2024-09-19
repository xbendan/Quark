#include <mixins/std/assert.h>
#include <quark/api/memory.h>
#include <quark/api/task.h>
#include <quark/sched/process.h>
#include <quark/sched/sched.h>

namespace Quark::System::API {
    using namespace Quark::System::Task;

    Res<RefPtr<Process>> createProcess(string name)
    {
        Process* p = new Process(
            Scheduler::nextPID(), name, createAddressSpace().unwrap());
        Process::addProcess(p);

        return Ok(RefPtr<Process>(p));
    }

    Res<RefPtr<Process>> createIdleProcess()
    {
        Process* p = new Process(Scheduler::nextPID(),
                                 "Idle",
                                 Process::getKernelProcess()->_addressSpace);
        Process::addProcess(p);

        return Ok(RefPtr<Process>(p));
    }

    template <typename... Args>
    Res<RefPtr<Process>> createProcessEx( //
        string  name,
        File*   file,
        Folder* workingDirectory,
        Args&&... launchArgs)
    {
        if (!file) {
            return Error::InvalidArgument();
        }

        Process* p = new Process(
            Scheduler::nextPID(), name, createAddressSpace().unwrap(), 0, 0, 0);
        Process::addProcess(p);

        return Error::NotImplemented();
    }

    Res<Thread*> createThread(Process* process)
    {
        return Error::NotImplemented();
    }

    Res<Thread*> createThreadEx(Process*      process,
                                u8            priority,
                                const string& name,
                                const string& description,
                                const string& command,
                                const string& arguments)
    {
        return Error::NotImplemented();
    }
}