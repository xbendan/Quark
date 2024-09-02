#pragma once

#include <mixins/meta/error.h>
#include <mixins/std/panic.h>

inline void
assert(bool expr, const char* msg = "Assertion failed")
{
    if (!expr) {
        Std::panic(msg);
    }
}

inline void
assert(bool expr, Error err)
{
    if (!expr) {
        Std::panic(err);
    }
}