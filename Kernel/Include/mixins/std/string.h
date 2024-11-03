#pragma once

#include <mixins/std/c++types.h>

extern "C"
{
    void* memset(void* p, u8 value, usize num) __attribute__((__nonnull__(1)));

    void* memcpy(void* dest, const void* src, usize num)
        __attribute__((__nonnull__(1, 2)));

    int memcmp(const void* s1, const void* s2, usize n)
        __attribute__((__nonnull__(1, 2)));

    usize strlen(const char* str) __attribute__((__nonnull__(1)));

    usize strnlen(const char* str, usize max = 512)
        __attribute__((__nonnull__(1)));
}