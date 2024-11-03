#include <platforms/x86_64/hwinterrupts.h>
#include <platforms/x86_64/tables.h>

#include <mixins/utils/array.h>
#include <quark/hal/interrupts.h>
#include <quark/os/diagnostic/logging.h>

namespace Quark::System::Platform::X64 {
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Hal;

    InterruptDescTbl           tbl = {};
    InterruptsControllerDevice controller;

    // clang-format off
    InterruptVector vectors[256] = {
        {
            ._name = "Division Error",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = DivisionError
        },
        {
            ._name = "Debug",
            ._traits = (InterruptVector::Fault | InterruptVector::Trap),
            ._hasErrorCode = false,
            ._handler = Debug
        },
        {
            ._name = "Non-maskable Interrupt",
            ._traits = InterruptVector::Interrupt,
            ._hasErrorCode = false,
            ._handler = NonMaskableInterrupt
        },
        {
            ._name = "Breakpoint",
            ._traits = InterruptVector::Trap,
            ._hasErrorCode = false,
            ._handler = Breakpoint
        },
        {
            ._name = "Overflow",
            ._traits = InterruptVector::Trap,
            ._hasErrorCode = false,
            ._handler = Overflow
        },
        {
            ._name = "Bound Range Exceeded",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = BoundRangeExceeded
        },
        {
            ._name = "Invalid Opcode",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = InvalidOpcode
        },
        {
            ._name = "Device Not Available",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = DeviceNotAvailable
        },
        {
            ._name = "Double Fault",
            ._traits = InterruptVector::Abort,
            ._hasErrorCode = true,
            ._handler = DoubleFault
        },
        {
            ._name = "Coprocessor Segment Overrun",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = CoprocessorSegmentOverrun
        },
        {
            ._name = "Invalid Task State Segment",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = InvalidTaskStateSegment
        },
        {
            ._name = "Segment Not Present",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = SegmentNotPresent
        },
        {
            ._name = "Stack-Segment Fault",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = StackSegmentFault
        },
        {
            ._name = "General Protection Fault",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = GeneralProtectionFault
        },
        {
            ._name = "Page Fault",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = PageFault
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "x87 Floating-Point Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = x87FloatingPointException
        },
        {
            ._name = "Alignment Check",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = AlignmentCheck
        },
        {
            ._name = "Machine Check",
            ._traits = InterruptVector::Abort,
            ._hasErrorCode = false,
            ._handler = MachineCheck
        },
        {
            ._name = "SIMD Floating-Point Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = SIMDFloatingPointException
        },
        {
            ._name = "Virtualization Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = VirtualizationException
        },
        {
            ._name = "Control Protection Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = ControlProtectionException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Reserved",
            ._traits = 0,
            ._hasErrorCode = false,
            ._handler = UnhandledException
        },
        {
            ._name = "Hypervisor Injection Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = false,
            ._handler = HypervisorInjectionException
        },
        {
            ._name = "VMM Communication Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = VMMCommunicationException
        },
        {
            ._name = "Security Exception",
            ._traits = InterruptVector::Fault,
            ._hasErrorCode = true,
            ._handler = SecurityException
        },
    };
    // clang-format on

    void UnhandledException(int num, Registers* registers)
    {
        // warn(u8"Unhandled exception %d\n", num);
    }

    void DivisionError(int num, Registers* registers)
    {
        // log(u8"Division Error\n");
    }

    void Debug(int num, Registers* registers) {}

    void NonMaskableInterrupt(int num, Registers* registers) {}

    void Breakpoint(int num, Registers* registers) {}

    void Overflow(int num, Registers* registers) {}

    void BoundRangeExceeded(int num, Registers* registers) {}

    void InvalidOpcode(int num, Registers* registers) {}

    void DeviceNotAvailable(int num, Registers* registers) {}

    void DoubleFault(int num, Registers* registers) {}

    void CoprocessorSegmentOverrun(int num, Registers* registers) {}

    void InvalidTaskStateSegment(int num, Registers* registers) {}

    void SegmentNotPresent(int num, Registers* registers) {}

    void StackSegmentFault(int num, Registers* registers) {}

    void GeneralProtectionFault(int num, Registers* registers) {}

    void PageFault(int num, Registers* registers) {}

    void x87FloatingPointException(int num, Registers* registers) {}

    void AlignmentCheck(int num, Registers* registers) {}

    void MachineCheck(int num, Registers* registers) {}

    void SIMDFloatingPointException(int num, Registers* registers) {}

    void VirtualizationException(int num, Registers* registers) {}

    void ControlProtectionException(int num, Registers* registers) {}

    void HypervisorInjectionException(int num, Registers* registers) {}

    void VMMCommunicationException(int num, Registers* registers) {}

    void SecurityException(int num, Registers* registers) {}

} // namespace Quark::System::Platform::X64

using Quark::System::Platform::X64::Registers;

extern "C"
{
    void isr_handler(int num, Registers* registers) {}

    void irq_handler(int num, Registers* registers) {}

    void ipi_handler(int num, Registers* registers) {}
}