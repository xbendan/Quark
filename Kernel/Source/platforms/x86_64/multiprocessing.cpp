#include <drivers/apic/device.h>
#include <mixins/meta/inert.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array_list.h>
#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/sched.h>
#include <platforms/x86_64/smp_define.inc>
#include <platforms/x86_64/tables.h>
#include <quark/api/device.h>
#include <quark/api/general.h>
#include <quark/api/memory.h>
#include <quark/api/task.h>
#include <quark/hal/multiprocessing.h>

namespace Quark::System {
    extern Inert<Platform::X64::CPULocal>  kCPULocal;
    extern Platform::X64::InterruptDescTbl kIdt;
}

namespace Quark::System::Platform::X64 {
    volatile u16* MagicValue       = (u16*)SMP_TRAMPOLINE_DATA_START_FLAG;
    volatile u16* TrampolineCpuID  = (u16*)SMP_TRAMPOLINE_CPU_ID;
    volatile u64* TrampolineCR3    = (u64*)SMP_TRAMPOLINE_CR3;
    volatile u64* TrampolineStack  = (u64*)SMP_TRAMPOLINE_STACK;
    volatile u64* TrampolineEntry2 = (u64*)SMP_TRAMPOLINE_ENTRY2;
    volatile bool DoneInit         = false;

    CPULocal* CPU0;

    GlobDescTbl::Pack* TrampolineGdtPack =
        (GlobDescTbl::Pack*)SMP_TRAMPOLINE_GDT_PTR;

    void TrampolineEntry(u16 cpuID)
    {
        CPULocal* cpu = (CPULocal*)Hal::getCPULocal(cpuID);

        setCPULocal(cpu);

        cpu->_gdt    = CPU0->_gdt;
        cpu->_gdtPtr = {
            ._size   = sizeof(cpu->_gdt) - 1,
            ._offset = (u64)&cpu->_gdt,
        };
        cpu->_idtPtr = {
            ._size   = sizeof(kIdt) - 1,
            ._offset = (u64)&kIdt,
        };
        cpu->_tss = TaskStateSegment(cpu->_gdt._tss);

        cpu->_schedule = {
            ._id            = cpuID,
            ._currentThread = nullptr,
            ._idleThread    = nullptr,
        };

        asm volatile("lgdt (%%rax)" ::"a"(&cpu->_gdtPtr));
        asm volatile("lidt %0" ::"m"(cpu->_idtPtr));

        getRegisteredDevice<APIC::GenericControllerDevice>(
            "Advanced Programmable Interrupt Controller")
            .get()
            ->getApicLocal(cpuID)
            ->setEnabled();

        asm volatile("sti");
        DoneInit = true;

        while (true)
            asm volatile("hlt; pause;");
    }
}

namespace Quark::System::Hal {
    using namespace Quark::System::Platform::X64;

    ArrayList<ICPULocal*>* _cpuLocals = nullptr;

    Res<IReadOnlyCollection<ICPULocal*>*> setupMultiprocessing()
    {
        auto* apic = getRegisteredDevice<APIC::GenericControllerDevice>(
                         "Advanced Programmable Interrupt Controller")
                         .take();
        assert(apic != nullptr,
               "Multi processing feature requires APIC device");

        assert(_cpuLocals == nullptr,
               "Do not call setupMultiprocessing() more than once");
        _cpuLocals = new ArrayList<ICPULocal*>();

        (*_cpuLocals)[0] = &(kCPULocal.unwrap());
        apic->getApicLocals()->forEach(
            [&](APIC::GenericControllerDevice::Local* apicLocal) {
                if (apicLocal->_apicId == 0)
                    return;

                *MagicValue       = 0;
                *TrampolineCpuID  = apicLocal->_apicId;
                *TrampolineEntry2 = (u64)TrampolineEntry;
                *TrampolineStack =
                    allocMemory4K(4,
                                  Process::getKernelProcess()->_addressSpace,
                                  Hal::VmmFlags::PRESENT |
                                      Hal::VmmFlags::WRITABLE)
                        .unwrap() +
                    4 * PAGE_SIZE_4K;
                *TrampolineGdtPack = CPU0->_gdtPtr;

                asm volatile("mov %%cr3, %%rax\n\t"
                             "mov %%rax, %0"
                             : "=m"(*TrampolineCR3)
                             :
                             : "rax");

                apicLocal->callIPI(ICR_DSH_DEST, ICR_MESSAGE_TYPE_INIT, 0);
                sleep(50);

                while (*MagicValue != 0xB33F) {
                    apicLocal->callIPI(ICR_DSH_DEST,
                                       ICR_MESSAGE_TYPE_STARTUP,
                                       (SMP_TRAMPOLINE_ENTRY >> 12));
                    sleep(200);
                }

                while (!DoneInit)
                    asm volatile("pause");

                DoneInit = false;
            });

        return Ok((IReadOnlyCollection<ICPULocal*>*)_cpuLocals);
    }

    ICPULocal* getCPULocal(u32 id)
    {
        return (*_cpuLocals)[id];
    }

    ICPULocal* getCPULocal()
    {
        return Platform::X64::getCPULocal();
    }

    void setCPULocal(u32 id, ICPULocal* local)
    {
        (*_cpuLocals)[id] = local;
    }

    void setCPULocal(ICPULocal* local)
    {
        Platform::X64::setCPULocal(
            static_cast<Platform::X64::CPULocal*>(local));
    }
}