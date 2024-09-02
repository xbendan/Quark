#pragma once

#include <mixins/std/c++types.h>

inline void
cli() {
    asm volatile("cli");
}

inline void
sti() {
    asm volatile("sti");
}

inline void
hlt() {
    asm volatile("hlt");
}

inline void
pause() {
    asm volatile("pause");
}

inline void
invlpg(u64 addr) {
    asm volatile("invlpg (%0)" ::"r"(addr)
                 : "memory");
}