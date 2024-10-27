#include <quark/memory/address_space.h>
#include <quark/sched/process.h>

namespace Quark::System::Task {
    using namespace Quark::System::Memory;

    Process::Process(u32           processId,
                     string        name,
                     AddressSpace* addressSpace,
                     u64           entryPoint,
                     u64           memHeap,
                     u64           memStack)
        : _processId(processId)
        , _name(name)
        , _addressSpace(addressSpace)
        , _mainThread(ProcessFactory::CreateThread(this))
        , m_childrenThreadList()
    {
        m_childrenThreadList.Add(_mainThread);
    }

    Process::~Process() {}

    Process* Process::GetKernelProcess()
    {
        return m_kernelProcess;
    }

    RefPtr<Process> Process::GetProcessById(u16 id)
    {
        return RefPtr<Process>(m_processes[id]);
    }

}