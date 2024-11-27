#include <quark/hal/ports.h>

namespace Vm::Qemu {
    using namespace Quark::System::Hal;

    static inline void Terminate()
    {
        out(0x604, 0x2000);
        PortAccess<0x604>().Out(0x2000);
    }
}