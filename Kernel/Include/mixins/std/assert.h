#pragma once

#include <mixins/fmt/format.h>
#include <mixins/meta/error.h>
#include <mixins/std/panic.h>

template <typename... Args>
static inline void
MakeAssertion(bool expr, const char* msg = "Assertion failed", Args&&... args)
{
    if (!expr) {
        Std::SystemPanic(msg, Std::forward<Args>(args)...);
    }
}

static inline void
MakeAssertion(bool expr, const char* msg = "Assertion failed")
{
    if (!expr) {
        Std::SystemPanic(msg);
    }
}

static inline void
MakeAssertion(bool expr, Error err)
{
    if (!expr) {
        Std::ThrowError(err);
    }
}