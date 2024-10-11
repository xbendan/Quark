#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/flags.h>
#include <platforms/x86_64/hwregs.h>

#define INT_DIVISION_ERROR 0
#define INT_DEBUG 1
#define INT_NON_MASKABLE_INTERRUPT 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUND_RANGE_EXCEEDED 5
#define INT_INVALID_OPCODE 6
#define INT_DEVICE_NOT_AVAILABLE 7
#define INT_DOUBLE_FAULT 8
#define INT_COPROCESSOR_SEGMENT_OVERRUN 9
#define INT_INVALID_TASK_STATE_SEGMENT 10
#define INT_SEGMENT_NOT_PRESENT 11
#define INT_STACK_SEGMENT_FAULT 12
#define INT_GENERAL_PROTECTION_FAULT 13
#define INT_PAGE_FAULT 14
#define INT_X87_FLOATING_POINT_EXCEPTION 16
#define INT_ALIGNMENT_CHECK 17
#define INT_MACHINE_CHECK 18
#define INT_SIMD_FLOATING_POINT_EXCEPTION 19
#define INT_VIRTUALIZATION_EXCEPTION 20
#define INT_CONTROL_PROTECTION_EXCEPTION 21
#define INT_HYPERVISOR_INJECTION_EXCEPTION 28
#define INT_VMM_COMMUNICATION_EXCEPTION 29
#define INT_SECURITY_EXCEPTION 30
#define IRQ_PIT_TIMER_UPDATE 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

namespace Quark::System::Platform::X64 {
    struct InterruptStackFrame : public Registers
    {
        u64 intno, err;
        u64 rip, cs, rflags, rsp, ss;
    } __attribute__((packed));

    struct InterruptVector
    {
        // string _name;
        const char* _name;

        enum Traits : u8
        {
            Reserved  = 0,
            Fault     = 0x1,
            Trap      = 0x2,
            Interrupt = 0x4,
            Abort     = 0x8
        };

        Flags<Traits> _traits;
        bool          _hasErrorCode;
        // Func<void(InterruptStackFrame*)> _handler;
        void (*_handler)(InterruptStackFrame*);
    };

    MakeFlags$(InterruptVector::Traits);

    void UnhandledException(InterruptStackFrame* context);

    void DivisionError(InterruptStackFrame* context);
    void Debug(InterruptStackFrame* context);
    void NonMaskableInterrupt(InterruptStackFrame* context);
    void Breakpoint(InterruptStackFrame* context);
    void Overflow(InterruptStackFrame* context);
    void BoundRangeExceeded(InterruptStackFrame* context);
    void InvalidOpcode(InterruptStackFrame* context);
    void DeviceNotAvailable(InterruptStackFrame* context);
    void DoubleFault(InterruptStackFrame* context);
    void CoprocessorSegmentOverrun(InterruptStackFrame* context);
    void InvalidTaskStateSegment(InterruptStackFrame* context);
    void SegmentNotPresent(InterruptStackFrame* context);
    void StackSegmentFault(InterruptStackFrame* context);
    void GeneralProtectionFault(InterruptStackFrame* context);
    void PageFault(InterruptStackFrame* context);
    void x87FloatingPointException(InterruptStackFrame* context);
    void AlignmentCheck(InterruptStackFrame* context);
    void MachineCheck(InterruptStackFrame* context);
    void SIMDFloatingPointException(InterruptStackFrame* context);
    void VirtualizationException(InterruptStackFrame* context);
    void ControlProtectionException(InterruptStackFrame* context);
    void HypervisorInjectionException(InterruptStackFrame* context);
    void VMMCommunicationException(InterruptStackFrame* context);
    void SecurityException(InterruptStackFrame* context);
} // namespace Quark::System::Platform::X64