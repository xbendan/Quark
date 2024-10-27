#pragma once

#include <mixins/fmt/format.h>
#include <mixins/meta/error.h>
#include <mixins/std/string.h>

namespace Std {
    [[noreturn]] void ThrowError(Error err);

    [[noreturn]] void SystemPanic(string msg);

    template <typename... Args>
        requires(fmt::Translatable<Std::RemoveCvRef<Args>> && ...)
    [[noreturn]] void SystemPanic(string msg, Args... args)
    {
        u8                 buffer[1024];
        BufferOutputStream buf(buffer, sizeof(buffer));
        f(buf, (const char*)msg.Data(), Std::forward<Args>(args)...);
        SystemPanic(string(buffer, string::LengthOf(buffer)));
    }
}