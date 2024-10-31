#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/str/string.h>
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
#define IRQ_PS2_KEYBOARD 33
#define IRQ_CASCADE 34
#define IRQ_COM2 35
#define IRQ_COM1 36
#define IRQ_LPT2 37
#define IRQ_FLOPPY_DISK 38
#define IRQ_LPT1 39
#define IRQ_CMOS_RTC_UPDATE 40
#define IRQ_PERIPHERALS_1 41
#define IRQ_PERIPHERALS_2 42
#define IRQ_PERIPHERALS_3 43
#define IRQ_PS2_MOUSE 44
#define IRQ_FPU_COPROCESSOR 45
#define IRQ_ATA_HARD_DISK_1 46
#define IRQ_ATA_HARD_DISK_2 47

namespace Quark::System::Platform::X64 {
    struct InterruptVector
    {
        // String _name;
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
        // Func<void(Registers*)> _handler;
        void (*_handler)(int, Registers*);
    };

    MakeFlags$(InterruptVector::Traits);

    void UnhandledException(int num, Registers* registers);

    void DivisionError(int num, Registers* registers);
    void Debug(int num, Registers* registers);
    void NonMaskableInterrupt(int num, Registers* registers);
    void Breakpoint(int num, Registers* registers);
    void Overflow(int num, Registers* registers);
    void BoundRangeExceeded(int num, Registers* registers);
    void InvalidOpcode(int num, Registers* registers);
    void DeviceNotAvailable(int num, Registers* registers);
    void DoubleFault(int num, Registers* registers);
    void CoprocessorSegmentOverrun(int num, Registers* registers);
    void InvalidTaskStateSegment(int num, Registers* registers);
    void SegmentNotPresent(int num, Registers* registers);
    void StackSegmentFault(int num, Registers* registers);
    void GeneralProtectionFault(int num, Registers* registers);
    void PageFault(int num, Registers* registers);
    void x87FloatingPointException(int num, Registers* registers);
    void AlignmentCheck(int num, Registers* registers);
    void MachineCheck(int num, Registers* registers);
    void SIMDFloatingPointException(int num, Registers* registers);
    void VirtualizationException(int num, Registers* registers);
    void ControlProtectionException(int num, Registers* registers);
    void HypervisorInjectionException(int num, Registers* registers);
    void VMMCommunicationException(int num, Registers* registers);
    void SecurityException(int num, Registers* registers);
} // namespace Quark::System::Platform::X64