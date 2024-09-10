#include <mixins/meta/error.h>
#include <mixins/std/string.h>

namespace Std {
    [[noreturn]] void panic(Error err);
    [[noreturn]] void panic(string msg);
}