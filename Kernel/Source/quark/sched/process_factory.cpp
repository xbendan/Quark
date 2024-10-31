#include <mixins/std/assert.h>
#include <quark/sched/process.h>
#include <quark/sched/sched.h>

namespace Quark::System::Task {
    Res<RefPtr<Process>> Process::CreateProcess(StringView name)
    {
        auto* p = new Process(Scheduler::GetDefaultPidNamespace().NextPid(),
                              name,
                              AddressSpace::Create().Unwrap());

        return Ok(RefPtr<Process>(p));
    }

    Res<RefPtr<Process>> Process::CreateIdleProcess()
    {
        auto* p = new Process(Scheduler::GetDefaultPidNamespace().NextPid(),
                              "Idle",
                              Process::GetKernelProcess()->_addressSpace);

        return Ok(RefPtr<Process>(p));
    }

    Res<Process*> Process::CreateKernelProcess(AddressSpace* addressSpace)
    {
        if (m_kernelProcess) {
            return Error::InvalidState("Kernel process already exists.");
        }

        m_kernelProcess = new Process(0, "Kernel", addressSpace, 0, 0, 0);

        m_processes    = new RefPtr<Process>[32];
        m_processes[0] = m_kernelProcess;

        return Ok(m_kernelProcess);
    }

    Res<RefPtr<Process>> Process::CreateProcessEx( //
        StringView name,
        File*      file,
        Folder*    workingDirectory,
        StringView launchArgs)
    {
        if (!file) {
            return Error::InvalidArgument();
        }

        auto* p = new Process(Scheduler::GetDefaultPidNamespace().NextPid(),
                              name,
                              AddressSpace::Create().Unwrap(),
                              0,
                              0,
                              0);

        return Ok(RefPtr<Process>(p));
    }
}