#pragma once

#include <mixins/std/c++types.h>

namespace Quark::Platform::X64 {
    struct Registers
    {
        u64 r15, r14, r13, r12, r11, r10, r9, r8;
        u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;
    } __attribute__((packed));
} // namespace Quark::Platform::X64