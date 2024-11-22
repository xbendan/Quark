#include <quark/memory/address_space.h>
#include <quark/sched/process.h>

namespace Quark::System::Task {
    using namespace Quark::System::Memory;

    Process::Process(u32            processId,
                     Qk::StringView name,
                     AddressSpace*  addressSpace,
                     u64            entryPoint,
                     u64            memHeap,
                     u64            memStack)
        : _processId(processId)
        , _name(name)
        , _addressSpace(addressSpace)
        , _mainThread(ProcessFactory::CreateThread(this))
        , m_childrenThreadList()
    {
        m_childrenThreadList.PushBack(_mainThread);
    }

    Process::~Process() {}

    bool Process::AddThread(Thread* thread)
    {
        if (thread->GetProcess() != nullptr) {
            return false;
        }

        m_childrenThreadList.PushBack(thread);
        return true;
    }

    Thread* Process::GetThreadById(u32 id)
    {
        return m_childrenThreadList
            .FindFirst(
                [id](Thread* thread) { return thread->GetThreadID() == id; })
            .Take();
    }

    u32 Process::GetNextThreadId()
    {
        return m_nextThreadId++;
    }

    Process* Process::GetKernelProcess()
    {
        return m_kernelProcess;
    }

    RefPtr<Process> Process::GetProcessById(u16 id)
    {
        return RefPtr<Process>(m_processes[id]);
    }

}