#include <mixins/meta/buf.h>

#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/paging.h>
#include <platforms/x86_64/sched.h>
#include <platforms/x86_64/tables.h>

#include <drivers/acpi/device.h>
#include <drivers/apic/device.h>
#include <drivers/pci/enumerator.h>

#include <quark/api/device.h>
#include <quark/api/logging.h>
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
    using namespace Quark::System::API;

    InterruptDescTbl                kIdt = {};
    Inert<CPULocal>                 kCPULocal;
    Buf<char[3 * 8 * PAGE_SIZE_4K]> kTssEntryBuf;
    Io::Device*                     kInitialDevices;

    Res<IReadOnlyList<Io::Device*>*> setupDevices()
    {
        log(u8"Setting up devices...");
        static ArrayList<Io::Device*> devices({
            new ACPI::ControllerDevice(),
            new APIC::GenericControllerDevice(),
            new PCI::PCIEnumerationDevice(),
        });
        return Ok((IReadOnlyList<Io::Device*>*)&devices);
    }

    Res<> setupArch(LaunchConfiguration* launchConfig)
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

        return Ok();
    }
} // namespace Quark::System