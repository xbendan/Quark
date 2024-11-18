#pragma once

#include <mixins/meta/event.h>
#include <mixins/meta/result.h>
#include <mixins/utils/list.h>
#include <quark/dev/device.h>
#include <quark/hal/definition.h>

#if defined(__x86_64__)
#include <platforms/x86_64/hwinterrupts.h>
#elif defined(__aarch64__)

#elif defined(__risv__)

#endif

namespace Quark::System::Hal {
    using Registers = Platform::__ARCH_NAMESPACE::Registers;

    using Qk::List;
    using Quark::System::Io::Device;

    bool EnableInterrupt(u8, bool (*)(int, Registers*));
    void DisableInterrupt(u8);

    Res<> SetupInterrupts();

    void EnableInterrupts();
    void DisableInterrupts();
}