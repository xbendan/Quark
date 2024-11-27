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

extern Quark::System::Platform::X64::GlobDescTbl::Pack GDT64Pack64;

namespace Quark::System {
    using namespace Quark::System::Memory;
    using namespace Quark::System::Platform::X64;
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Hal;
    using Qk::Array;

    InterruptDescTbl::Entry kIdtEntries[256] = {};
    InterruptDescTbl::Pack  kIdtPtr          = {};
    Inert<CPULocalDevice>   kCPULocal;

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

        for (int i = 0; i < 256; i++)
            kIdtEntries[i] = InterruptDescTbl::Entry(
                IntVec[i], 0x08, InterruptDescTbl::IntGate);

        kIdtEntries[8]._ist = 2;

        // p->_idtPtr = {
        //     sizeof(InterruptDescTbl::Entry) * InterruptDescTbl::Count - 1,
        //     (u64)&kIdtEntries,
        // };
        kIdtPtr = {
            sizeof(InterruptDescTbl::Entry) * InterruptDescTbl::Count - 1,
            (u64)&kIdtEntries,
        };
        _lidt(&kIdtPtr);

        out<u8>(0x20, 0x11);
        out<u8>(0xA0, 0x11);
        out<u8>(0x21, 0x20);
        out<u8>(0xA1, 0x28);
        out<u8>(0x21, 0x04);
        out<u8>(0xA1, 0x02);
        out<u8>(0x21, 0x01);
        out<u8>(0xA1, 0x01);
        out<u8>(0x21, 0x0);
        out<u8>(0xA1, 0x0);

        /* load global descriptor table */
        _lgdt(&p->_gdtPtr);

        SetCPULocal(p);

        // Check if the system supports the required features
        CPUID cpuid(1, 0);
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