#pragma once

#include <mixins/concurrent/spinlock.h>
#include <mixins/fmt/translator.h>
#include <mixins/meta/buf.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/string.h>

template <typename Arg>
    requires(fmt::Translatable<Std::RemoveCvRef<Arg>>)
static inline void
__translate(BufferOutputStream& stream, Arg arg)
{
    fmt::Translator<string::Unit, Std::RemoveCvRef<Arg>>::ToUnitType(stream,
                                                                     arg);
}

template <typename... Args>
    requires(fmt::Translatable<Std::RemoveCvRef<Args>> && ...)
static inline void
f(BufferOutputStream& buf, const char* raw, Args&&... args)
{
    int len = string::LengthOf(raw);

    Tuple<Args...> tuple{ args... };

    int i = 0;
    for (int j; i < len; i++) {
        if (raw[i] == '{' && raw[i + 1] == '}') {
            __translate(buf, tuple[j]);
            j++;
            i++;
        } else {
            buf << raw[i];
        }
    }

    buf << '\0';
}
