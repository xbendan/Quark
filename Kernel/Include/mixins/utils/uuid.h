#pragma once

#include <mixins/std/c++types.h>
#include <mixins/str/string.h>

namespace Qk {
}

union UUID
{
    u8  _bytes[16];
    u16 _words[8];
    u32 _dwords[4];
    u64 _qwords[2];

    constexpr inline bool operator==(UUID const& other) const
    {
        return _qwords[0] == other._qwords[0] && _qwords[1] == other._qwords[1];
    }

    constexpr inline bool operator!=(UUID const& other) const
    {
        return !(*this == other);
    }

    constexpr inline bool IsNull() const
    {
        return _qwords[0] == 0 && _qwords[1] == 0;
    }

    static UUID FromName(Qk::StringView name)
    {
        UUID uuid;
        for (usize i = 0; i < name.len(); i++) {
            uuid._bytes[i % 16] ^= name[i];
        }
        return uuid;
    }
} __attribute__((packed));