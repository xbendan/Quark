#include <quark/api/task.h>
#include <quark/memory/address_space.h>
#include <quark/sched/process.h>

namespace Quark::System::Task {
    using namespace Quark::System::Memory;

    Process* _kernelProcess;

    Process::Process(u32           processId,
                     string        name,
                     AddressSpace* addressSpace,
                     u64           entryPoint,
                     u64           memHeap,
                     u64           memStack)
        : _processId(processId)
        , _name(name)
        , _addressSpace(addressSpace)
        , _mainThread(API::createThread(this).unwrap())
        , m_childrenThreadList()
    {
    }

    Process::~Process() {}

    Process* Process::getKernelProcess()
    {
        return _kernelProcess;
    }
}

namespace Quark::System {
    using Quark::System::Memory::AddressSpace;
    using Quark::System::Task::Process;

    Res<Process*> createKernelProcess(AddressSpace* addressSpace)
    {
        if (Task::_kernelProcess)
            return Error::InvalidState("Kernel process already exists.");

        Task::_kernelProcess = new Process(0, "Kernel", addressSpace, 0, 0, 0);
        Process::addProcess(Task::_kernelProcess);

        return Ok(Task::_kernelProcess);
    }
}