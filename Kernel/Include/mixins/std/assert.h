#pragma once

#include <mixins/meta/error.h>
#include <mixins/std/panic.h>

static inline void
MakeAssertion(bool expr, const char* msg = "Assertion failed")
{
    if (!expr) {
        Std::panic(msg);
    }
}

static inline void
MakeAssertion(bool expr, Error err)
{
    if (!expr) {
        Std::panic(err);
    }
}