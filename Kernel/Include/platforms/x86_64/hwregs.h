#pragma once

#include <mixins/std/c++types.h>

namespace Quark::System::Platform::X64 {
    struct Registers
    {
        u64 r15, r14, r13, r12, r11, r10, r9, r8;
        u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;
        u64 err;
        u64 rip, cs, rflags, rsp, ss;

        void DumpAll();
    } __attribute__((packed));
} // namespace Quark::System::Platform::X64