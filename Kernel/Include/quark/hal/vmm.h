#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>
#include <quark/hal/definition.h>

#if defined(__x86_64__)
#include <platforms/x86_64/paging.h>
#elif defined(__aarch64__)

#elif defined(__risv__)

#endif

namespace Quark::System::Hal {
    using VmmFlags = Quark::System::Platform::__ARCH_NAMESPACE::VmmFlags;

    Res<> InitVirtMemory();
} // namespace Quark::System::Hal