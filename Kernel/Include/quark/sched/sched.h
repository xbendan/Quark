#pragma once

#include <mixins/meta/ref.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <quark/sched/process.h>

namespace Quark::Task {

    class Scheduler
    {
    public:
        struct Context
        {
            u32     _id;
            Thread* _currentThread;
            Thread* _idleThread;
        };

        Scheduler()
            : m_processes()
            , m_nextPID(1)
        {
            static Process kernelProcess(0, "Kernel", nullptr);

            m_kernelProcess = &kernelProcess;
        }

        void     schedule(Context* context, Thread* thread);
        Context* context();

        Process*        getKernelProcess() { return m_kernelProcess; }
        void            addProcess(Process* process);
        RefPtr<Process> getProcessById(u16 id);

        u16 nextPID() { return m_nextPID++; }

    private:
        Process*               m_kernelProcess;
        Array<RefPtr<Process>> m_processes;
        u16                    m_nextPID;
    };
} // namespace Quark::Task