#if defined(__x86_64__)

#include <platforms/x86_64/hwinterrupts.h>
#define __ARCH_NAMESPACE X64

#elif defined(__aarch64__)

#elif defined(__risv__)

#endif

namespace Quark::System::Platform {
    using InterruptStackFrame = __ARCH_NAMESPACE::InterruptStackFrame;
}