#include <platforms/x86_64/tables.h>
#include <quark/memory/page_alloc.h>
#include <quark/os/main.h>

namespace Quark::System::Platform::X64 {

    TaskStateSegment::TaskStateSegment(u64 istp)
        : _ist{ istp + STACK_SIZE * 1,
                istp + STACK_SIZE * 2,
                istp + STACK_SIZE * 3 }
    {
        asm volatile("mov %%rsp, %0" : "=r"(_rsp[0]));
        asm volatile("ltr %%ax" ::"a"(0x2b));
    }
}