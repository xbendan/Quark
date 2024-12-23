#include <drivers/apic/device.h>
#include <mixins/meta/inert.h>
#include <mixins/std/c++types.h>
#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/sched.h>
#include <platforms/x86_64/smp_define.inc>
#include <platforms/x86_64/tables.h>
#include <quark/dev/device.h>
#include <quark/hal/multiprocessing.h>
#include <quark/memory/page_alloc.h>
#include <quark/os/diagnostic/logging.h>
#include <quark/sched/completable.h>

namespace Quark::System {
    extern Inert<Platform::X64::CPULocalDevice>  kCPULocal;
    extern Platform::X64::InterruptDescTbl::Pack kIdtPtr;
}

extern void* SMPTrampolineStart;
extern void* SMPTrampolineEnd;

extern Quark::System::Platform::X64::GlobDescTbl::Pack GDT64Pack64;

namespace Quark::System::Platform::X64 {
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Io;
    using namespace Quark::System::Memory;
    using namespace Quark::System::Task;

    volatile u16* MagicValue       = (u16*)SMP_TRAMPOLINE_DATA_START_FLAG;
    volatile u16* TrampolineCpuID  = (u16*)SMP_TRAMPOLINE_CPU_ID;
    volatile u64* TrampolineCR3    = (u64*)SMP_TRAMPOLINE_CR3;
    volatile u64* TrampolineStack  = (u64*)SMP_TRAMPOLINE_STACK;
    volatile u64* TrampolineEntry2 = (u64*)SMP_TRAMPOLINE_ENTRY2;
    volatile bool DoneInit         = false;

    CPULocalDevice* CPU0;

    GlobDescTbl::Pack* TrampolineGdtPack =
        (GlobDescTbl::Pack*)SMP_TRAMPOLINE_GDT_PTR;

    void TrampolineEntry(u16 cpuID)
    {
        info$("[SMP] CPU {} is being initialized.", cpuID);
        CPULocalDevice* cpu = (CPULocalDevice*)Hal::GetCPULocal(cpuID);

        SetCPULocal(cpu);

        cpu->_gdt    = CPU0->_gdt;
        cpu->_gdtPtr = {
            ._size   = sizeof(cpu->_gdt) - 1,
            ._offset = (u64)&cpu->_gdt,
        };

        cpu->_idtPtr = kIdtPtr;
        asm volatile("lidt %0" ::"m"(kIdtPtr));

        cpu->_gdt._tss = GlobDescTbl::TssEntry(&cpu->_tss);
        asm volatile("lgdt (%%rax)" ::"a"(&cpu->_gdtPtr));

        cpu->_tss = TaskStateSegment(
            Memory::AllocateMemory4K(STACK_SIZE / PAGE_SIZE_4K * 3,
                                     Process::GetKernelProcess()->_addressSpace,
                                     Hal::VmmFlags::PRESENT |
                                         Hal::VmmFlags::WRITABLE)
                .unwrap());

        Device::FindByName<APIC::GenericControllerDevice>(
            "Advanced Programmable Interrupt Controller")
            .Get()
            ->GetApicLocal(cpuID)
            ->Enable();

        // asm volatile("sti");
        info$("[SMP] CPU {} is initialized.", cpuID);
        DoneInit = true;

        while (true)
            asm volatile("hlt; pause;");
    }
}

namespace Quark::System::Hal {
    using namespace Quark::System::Platform::X64;
    using Qk::List;

    List<ICPULocalDevice*>* _cpuLocals = nullptr;

    Res<List<ICPULocalDevice*>*> SetupMultiprocessing()
    {
        auto apicRes = Device::FindByName<APIC::GenericControllerDevice>(
            "Advanced Programmable Interrupt Controller");
        assert(apicRes.IsPresent(),
               "Multi processing feature requires APIC device");

        auto* apic = apicRes.Take();
        assert(_cpuLocals == nullptr,
               "Do not call SetupMultiprocessing() more than once");
        _cpuLocals = apic->GetApicLocals().Select<ICPULocalDevice*>(
            [](APIC::GenericControllerDevice::Local* const& apicLocal) {
                return apicLocal->_device;
            });

        memcpy((void*)SMP_TRAMPOLINE_ENTRY,
               (void*)&SMPTrampolineStart,
               (u64)&SMPTrampolineEnd - (u64)&SMPTrampolineStart);

        (*_cpuLocals)[0] = &(kCPULocal.unwrap());
        for (APIC::GenericControllerDevice::Local* apicLocal :
             apic->GetApicLocals()) {
            if (apicLocal->_apicId == (*_cpuLocals)[0]->_id) {
                CPU0 = static_cast<CPULocalDevice*>(apicLocal->_device);
                continue;
            }

            info$("[SMP] Initializing CPU {}", apicLocal->_processorId);
            *MagicValue       = 0;
            *TrampolineCpuID  = apicLocal->_processorId;
            *TrampolineEntry2 = (u64)TrampolineEntry;
            *TrampolineStack =
                AllocateMemory4K(4,
                                 Process::GetKernelProcess()->_addressSpace,
                                 Hal::VmmFlags::PRESENT |
                                     Hal::VmmFlags::WRITABLE)
                    .unwrap() +
                4 * PAGE_SIZE_4K;
            // *TrampolineGdtPack = CPU0->_gdtPtr;
            *TrampolineGdtPack = GDT64Pack64;

            asm volatile("mov %%cr3, %%rax\n\t"
                         "mov %%rax, %0"
                         : "=m"(*TrampolineCR3)
                         :
                         : "rax");

            apicLocal->SendIPI(ICR_DSH_DEST, ICR_MESSAGE_TYPE_INIT, 0);
            Task::Delay(50);

            while (*MagicValue != 0xB33F) {
                apicLocal->SendIPI(ICR_DSH_DEST,
                                   ICR_MESSAGE_TYPE_STARTUP,
                                   (SMP_TRAMPOLINE_ENTRY >> 12));
                Task::Delay(200);
            }

            while (!DoneInit)
                asm volatile("pause");

            DoneInit = false;
        }

        CPU0->_gdt._tss = GlobDescTbl::TssEntry(&CPU0->_tss);
        CPU0->_tss      = TaskStateSegment(
            AllocateMemory4K(STACK_SIZE / PAGE_SIZE_4K * 3,
                             Process::GetKernelProcess()->_addressSpace,
                             Hal::VmmFlags::PRESENT | Hal::VmmFlags::WRITABLE)
                .unwrap());

        return Ok(_cpuLocals);
    }

    ICPULocalDevice* GetCPULocal(u32 id)
    {
        return (*_cpuLocals)[id];
    }

    ICPULocalDevice* GetCPULocal()
    {
        return Platform::X64::GetCPULocal();
    }

    void SetCPULocal(u32 id, ICPULocalDevice* local)
    {
        (*_cpuLocals)[id] = local;
    }

    void SetCPULocal(ICPULocalDevice* local)
    {
        Platform::X64::SetCPULocal(
            static_cast<Platform::X64::CPULocalDevice*>(local));
    }

    void ICPULocalDevice::SendSignal(Signal signal, u32 data)
    {
        // switch (signal) {
        //     case Signal::SCHED:
        //         asm volatile("int $0x80");
        //         break;
        //     default:
        //         break;
        // }
    }
}