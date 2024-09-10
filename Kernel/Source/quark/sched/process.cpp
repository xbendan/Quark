#include <quark/api/task.h>
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
        , _mainThread(API::createThread(this).unwrap())
        , m_childrenThreadList()
    {
    }

    Process::~Process() {}
}