#include <platforms/x86_64/tables.h>
#include <quark/api/memory.h>
#include <quark/api/task.h>
#include <quark/os/main.h>

namespace Quark::System::Platform::X64 {
    TaskStateSegment::TaskStateSegment(GlobDescTbl::TssEntry& tss)
    {
        tss = GlobDescTbl::TssEntry(this);

        for (int i = 0; i < 3; i++) {
            _ist[i] =
                allocMemory4K(8, Process::getKernelProcess()->_addressSpace)
                    .unwrap();
            _ist[i] += PAGE_SIZE_4K * 8;
        }
        asm volatile("mov %%rsp, %0" : "=r"(_rsp[0]));
        asm volatile("ltr %%ax" ::"a"(0x28));
    }

    GlobDescTbl::TssEntry::TssEntry(TaskStateSegment* tss)
        : _len(sizeof(TaskStateSegment) - 1)
        , _baseLow((u64)tss & 0xffff)
        , _baseMedium(((u64)tss >> 16) & 0xff)
        , _flags0(0x89)
        , _flags1(0x00)
        , _baseHigh(((u64)tss >> 24) & 0xff)
        , _baseHighest(((u64)tss >> 32) & 0xffffffff)
        , __reserved__0(0)
    {
    }
}