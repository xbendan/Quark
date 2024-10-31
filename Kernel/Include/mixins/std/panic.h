#pragma once

#include <mixins/meta/error.h>

namespace Std {
    [[noreturn]] void panic(Error err);
    [[noreturn]] void panic(const char* msg);
}