#include <platforms/x86_64/hwinterrupts.h>
#include <platforms/x86_64/tables.h>

#include <mixins/utils/array.h>
#include <quark/hal/interrupts.h>
#include <quark/os/diagnostic/logging.h>

#include <drivers/apic/device.h>
#include <drivers/apic/interrupt.h>

extern u64 IntVec[256];

namespace Quark::System::Platform::X64 {
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Hal;

    InterruptDescTbl IntTbls;

    bool (*Interrupts[0xFF])(int, Registers*) = {
        [0]  = DivisionError,
        [1]  = Debug,
        [2]  = NonMaskableInterrupt,
        [3]  = Breakpoint,
        [4]  = Overflow,
        [5]  = BoundRangeExceeded,
        [6]  = InvalidOpcode,
        [7]  = DeviceNotAvailable,
        [8]  = DoubleFault,
        [9]  = CoprocessorSegmentOverrun,
        [10] = InvalidTaskStateSegment,
        [11] = SegmentNotPresent,
        [12] = StackSegmentFault,
        [13] = GeneralProtectionFault,
        [14] = PageFault,
        [15] = UnhandledException,
        [16] = x87FloatingPointException,
        [17] = AlignmentCheck,
        [18] = MachineCheck,
        [19] = SIMDFloatingPointException,
        [20] = VirtualizationException,
        [21] = ControlProtectionException,
        [22] = UnhandledException,
        [23] = UnhandledException,
        [24] = UnhandledException,
        [25] = UnhandledException,
        [26] = UnhandledException,
        [27] = UnhandledException,
        [28] = HypervisorInjectionException,
        [29] = VMMCommunicationException,
        [30] = SecurityException,
    };

    void Registers::DumpAll()
    {
        info("<------------= REGISTERS =------------>");
        info$("RAX[{:#X}], RBX[{:#X}], RCX[{:#X}]",
              Qk::align(rax, Qk::Align::RIGHT, 18),
              Qk::align(rbx, Qk::Align::RIGHT, 18),
              Qk::align(rcx, Qk::Align::RIGHT, 18));
        info$("RDX[{:#X}], RSI[{:#X}], RDI[{:#X}]",
              Qk::align(rdx, Qk::Align::RIGHT, 18),
              Qk::align(rsi, Qk::Align::RIGHT, 18),
              Qk::align(rdi, Qk::Align::RIGHT, 18));
        info$("RSP[{:#X}], RBP[{:#X}], RIP[{:#X}]",
              Qk::align(rsp, Qk::Align::RIGHT, 18),
              Qk::align(rbp, Qk::Align::RIGHT, 18),
              Qk::align(rip, Qk::Align::RIGHT, 18));
        info$("R8 [{:#X}], R9 [{:#X}], R10[{:#X}]",
              Qk::align(r8, Qk::Align::RIGHT, 18),
              Qk::align(r9, Qk::Align::RIGHT, 18),
              Qk::align(r10, Qk::Align::RIGHT, 18));
        info$("R11[{:#X}], R12[{:#X}], R13[{:#X}]",
              Qk::align(r11, Qk::Align::RIGHT, 18),
              Qk::align(r12, Qk::Align::RIGHT, 18),
              Qk::align(r13, Qk::Align::RIGHT, 18));
        info$("R14[{:#X}], R15[{:#X}]",
              Qk::align(r14, Qk::Align::RIGHT, 18),
              Qk::align(r15, Qk::Align::RIGHT, 18));
        info$("Flags[{:#b}]", rflags);
        info(" ");
        info("Stack Trace:");
        // TODO: Print stack trace
        info("<------------------------------------->");
    }

    bool UnhandledException(int num, Registers* registers)
    {
        error$("[INT] Unhandled interrupt: {}\n", num);
        return false;
    }

    bool DivisionError(int num, Registers* registers)
    {
        warn("[INT] Division Error");

        Std::panic("Division Error");
    }

    bool Debug(int num, Registers* registers)
    {
        return false;
    }

    bool NonMaskableInterrupt(int num, Registers* registers)
    {
        return false;
    }

    bool Breakpoint(int num, Registers* registers)
    {
        return false;
    }

    bool Overflow(int num, Registers* registers)
    {
        return false;
    }

    bool BoundRangeExceeded(int num, Registers* registers)
    {
        return false;
    }

    bool InvalidOpcode(int num, Registers* registers)
    {
        return false;
    }

    bool DeviceNotAvailable(int num, Registers* registers)
    {
        return false;
    }

    bool DoubleFault(int num, Registers* registers)
    {
        return false;
    }

    bool CoprocessorSegmentOverrun(int num, Registers* registers)
    {
        return false;
    }

    bool InvalidTaskStateSegment(int num, Registers* registers)
    {
        return false;
    }

    bool SegmentNotPresent(int num, Registers* registers)
    {
        return false;
    }

    bool StackSegmentFault(int num, Registers* registers)
    {
        return false;
    }

    bool GeneralProtectionFault(int num, Registers* registers)
    {
        info("[INT] --- General Protection Fault --- ");
        info$("[INT] Error Code: {:#X}", registers->err);
        registers->DumpAll();

        Std::panic("General Protection Fault");
        return false;
    }

    bool PageFault(int num, Registers* registers)
    {
        info("[INT] --- Page Fault --- ");
        bool p = registers->err & 1, w = registers->err & 2,
             u = registers->err & 3;
        u64 addr;
        asm volatile("mov %%cr2, %0" : "=r"(addr));

        info$("[INT] P[{}], W[{}], U[{}]",
              Qk::align(p, Qk::Align::LEFT, 5),
              Qk::align(w, Qk::Align::LEFT, 5),
              Qk::align(u, Qk::Align::LEFT, 5));
        info$("[INT] Address: {:#X}", addr);

        return false;
    }

    bool x87FloatingPointException(int num, Registers* registers)
    {

        return false;
    }

    bool AlignmentCheck(int num, Registers* registers)
    {
        return false;
    }

    bool MachineCheck(int num, Registers* registers)
    {
        return false;
    }

    bool SIMDFloatingPointException(int num, Registers* registers)
    {
        return false;
    }

    bool VirtualizationException(int num, Registers* registers)
    {
        return false;
    }

    bool ControlProtectionException(int num, Registers* registers)
    {
        return false;
    }

    bool HypervisorInjectionException(int num, Registers* registers)
    {
        return false;
    }

    bool VMMCommunicationException(int num, Registers* registers)
    {
        return false;
    }

    bool SecurityException(int num, Registers* registers)
    {
        return false;
    }

} // namespace Quark::System::Platform::X64

using namespace Quark::System::Platform::X64;

extern "C"
{
    void isr_handler(int num, Registers* registers)
    {
        if (__builtin_expect(Interrupts[num] != nullptr, 1) &&
            Interrupts[num](num, registers) == true) {
            return;
        }

        warn$("Unhandled or unresolved interrupt: {}", num);
        if (!(registers->ss & 0x3)) /* Kernel Interrupt Exception */ {
            Device::FindByName<APIC::GenericControllerDevice>(
                APIC::GenericControllerDevice::Name)
                .IfPresent([](auto* apic) {
                    apic->GetApicLocal(0)->SendIPI(
                        ICR_DSH_OTHER, ICR_MESSAGE_TYPE_FIXED, IPI_HALT);
                });

            // TODO: BSOD
            registers->DumpAll();
            Std::panic("Unresolvable Kernel Interrupt Exception.");
        } else /* User Process Interrupt Exception */ {
        }
    }

    void irq_handler(int num, Registers* registers)
    {
        if (__builtin_expect(Interrupts[num] != nullptr, 1) &&
            Interrupts[num](num, registers) == true) {

            APIC::EndOfInterrupt();
            return;
        } else {
            warn$("Unhandled or unresolved interrupt: {}", num);
        }
    }

    void ipi_handler(int num, Registers* registers)
    {
        APIC::EndOfInterrupt();
        if (__builtin_expect(Interrupts[num] != nullptr, 1) &&
            Interrupts[num](num, registers) == true) {
            return;
        } else {
            warn$("Unhandled or unresolved interrupt: {}", num);
        }
    }
}

namespace Quark::System::Hal {
    bool EnableInterrupt(u8 num, bool (*handler)(int, Registers*))
    {
        if (num < 0x20) {
            warn$("Trying to set handler for invalid interrupt number: {}",
                  num);
            return false;
        }

        APIC::EnableInterrupt(num);

        Interrupts[num] = handler;
        return true;
    }

    void EnableInterrupts()
    {
        asm volatile("sti");
    }

    void DisableInterrupts()
    {
        asm volatile("cli");
    }
}