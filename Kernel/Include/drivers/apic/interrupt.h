#include <mixins/std/c++types.h>

namespace APIC {
    void EndOfInterrupt();

    void EnableInterrupt(u8 irq, u64 data);

    void EnableInterrupt(u8 irq);
}