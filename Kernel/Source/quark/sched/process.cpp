#include <quark/api/task.h>
#include <quark/sched/process.h>

namespace Quark::Task {
    using namespace Quark::Mem;

    Process::Process(u32           processId,
                     String<>      name,
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