#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/flags.h>
#include <platforms/x86_64/hwregs.h>

namespace Quark::Platform::X64 {
    struct InterruptStackFrame : public Registers
    {
        u64 intno, err;
        u64 rip, cs, rflags, rsp, ss;
    } __attribute__((packed));

    struct InterruptVector
    {
        // String<Utf8> _name;
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
} // namespace Quark::Platform::X64