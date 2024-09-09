#include <mixins/meta/buf.h>

#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/paging.h>
#include <platforms/x86_64/sched.h>
#include <platforms/x86_64/tables.h>

#include <quark/memory/address_range.h>
#include <quark/os/main.h>

extern "C" void
_lgdt(void*);
extern "C" void
_lidt(void*);

extern u64 interruptVectors[];

namespace Quark::System {
    using namespace Quark::System::Memory;
    using namespace Quark::System::Platform::X64;

    InterruptDescTbl                kIdt = {};
    Inert<CPULocal>                 kCPULocal;
    Buf<char[3 * 8 * PAGE_SIZE_4K]> kTssEntryBuf;

    Res<> setupArch(OsBootConfig* bootInfo)
    {
        asm volatile("cli");

        CPULocal* p = kCPULocal(0);

        /* load global descriptor table */
        _lgdt(&p->_gdtPtr);

        /* load interrupt descriptor table */
        for (int i = 0; i < kIdt.Count; i++) {
            kIdt._entries[i] =
                InterruptDescTbl::Entry((u64)&interruptVectors[i],        //
                                        0x08,                             //
                                        0,                                //
                                        InterruptDescTbl::InterruptGate); //
        }
        p->_idtPtr = { sizeof(kIdt) - 1, (u64)&kIdt };
        _lidt(&p->_idtPtr);

        setCPULocal(p);

        p->_gdt._tss = GlobDescTbl::TssEntry(&p->_tss);
        for (int i = 0; i < 3; i++) {
            p->_tss._ist[i] = (u64)&kTssEntryBuf._data[i * 8 * PAGE_SIZE_4K];
            // memset
            p->_tss._ist[i] += PAGE_SIZE_4K * 8;
        }

        asm volatile("mov %%rsp, %0" : "=r"(p->_tss._rsp[0]));
        asm volatile("ltr %%ax" ::"a"(0x28));

        // setup virtual memory managements

        // Setup tasking
        // Setup logging
        // Load devices
        // Load filesystems

        return Ok();
    }
} // namespace Quark::System