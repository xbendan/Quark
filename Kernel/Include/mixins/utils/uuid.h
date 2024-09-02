#pragma once

#include <mixins/std/c++types.h>

union UUID
{
    u8  _bytes[16];
    u16 _words[8];
    u32 _dwords[4];
    u64 _qwords[2];

    constexpr inline bool operator==(const UUID& other) const
    {
        return _qwords[0] == other._qwords[0] && _qwords[1] == other._qwords[1];
    }
} __attribute__((packed));