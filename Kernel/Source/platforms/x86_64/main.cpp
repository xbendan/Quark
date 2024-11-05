#include <mixins/meta/buf.h>

#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/paging.h>
#include <platforms/x86_64/sched.h>
#include <platforms/x86_64/tables.h>

#include <drivers/acpi/device.h>
#include <drivers/apic/device.h>
#include <drivers/clocksource/pit/device.h>
#include <drivers/cmos/timer_device.h>
#include <drivers/pci/enumeration.h>
#include <drivers/ps2/device.h>

#include <quark/memory/address_range.h>
#include <quark/os/diagnostic/logging.h>
#include <quark/os/main.h>

extern "C"
{
    void _lgdt(void*);
    void _lidt(void*);
    void _lfpu();
}

extern u64 IntVec[];

namespace Quark::System {
    using namespace Quark::System::Memory;
    using namespace Quark::System::Platform::X64;
    using namespace Quark::System::Diagnostic;
    using Qk::Array;

    InterruptDescTbl                    kIdt = {};
    Inert<CPULocalDevice>               kCPULocal;
    Qk::Buf<char, 3 * 8 * PAGE_SIZE_4K> kTssEntryBuf;

    Res<Array<Io::Device*>*> EnumerateInitialDevices()
    {
        log("Setting up devices...");
        auto* devices = new Array<Io::Device*>({
            new ACPI::ControllerDevice(),
            new APIC::GenericControllerDevice(),
            new PCI::PCIEnumerationDevice(),
            new PS2::LegacyControllerDevice(),
            new PIT::PITimerDevice(1000),
            new CMOS::RealTimeClockDevice(),
        });

        return Ok(devices);
    }

    Res<> SetupArch()
    {
        asm volatile("cli");

        CPULocalDevice* p = kCPULocal(0);

        /* load global descriptor table */
        _lgdt(&p->_gdtPtr);

        /* load interrupt descriptor table */
        // for (int i = 0; i < kIdt.Count; i++) {
        //     kIdt.Entries[i] =
        //         InterruptDescTbl::Entry((u64)&interruptVectors[i],  //
        //                                 0x08,                       //
        //                                 0,                          //
        //                                 InterruptDescTbl::IntGate); //
        // }
        for (int i = 0; i < 256; i++)
            kIdt.Entries[i] = InterruptDescTbl::Entry(
                IntVec[i], 0x08, InterruptDescTbl::IntGate);

        kIdt.Entries[8]._ist = 2;

        p->_idtPtr = { sizeof(kIdt) - 1, (u64)&kIdt };
        _lidt(&p->_idtPtr);

        SetCPULocal(p);

        p->_gdt._tss = GlobDescTbl::TssEntry(&p->_tss);
        for (int i = 0; i < 3; i++) {
            p->_tss._ist[i] = (u64)&kTssEntryBuf._data[i * 8 * PAGE_SIZE_4K];
            // memset
            p->_tss._ist[i] += PAGE_SIZE_4K * 8;
        }

        asm volatile("mov %%rsp, %0" : "=r"(p->_tss._rsp[0]));
        asm volatile("ltr %%ax" ::"a"(0x28));

        // Check if the system supports the required features
        CPUID cpuid(1, 0);
        // u32   eax, ebx, ecx, edx;
        // asm volatile("cpuid"
        //              : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        //              : "a"(1), "c"(0));
        assert(cpuid._ecx & CPUID_ECX_SSE4_2,
               "System requires [SSE4.2] to run while it's not supported");
        assert(cpuid._edx & CPUID_EDX_FPU,
               "System requires [FPU] to run while it's not supported."
               "FPUs are coprocessors that are used to perform floating-point"
               "operations");
        assert(cpuid._edx & CPUID_EDX_APIC,
               "System requires [APIC] to run while it's not supported, "
               "The Advanced Programmable Interrupt Controller (APIC) is a "
               "family of interrupt controllers");
        // WTF is this?
        // Why the CPUID provided by QEMU doesn't have ACPI bit?
        // assert(edx & CPUID_EDX_ACPI,
        //        "System requires [ACPI] to run while it's not supported, "
        //        "The Advanced Configuration and Power Interface (ACPI) is an "
        //        "open industry specification that defines power management "
        //        "and configuration interfaces between operating systems and "
        //        "hardware");
        assert(cpuid._edx & CPUID_EDX_MSR,
               "System requires [MSR] to run while it's not supported, "
               "Model-specific registers (MSRs) are special registers in "
               "the x86 architecture");

        // Enable FPU here
        // _lfpu();

        // u32 cr4 = rdcr4();
        // cr4 |= (CR4_OSFXSR |     //
        //         CR4_OSXMMEXCPT | //
        //         CR4_OSXSAVE);
        // wrcr4(cr4);

        return Ok();
    }
} // namespace Quark::System