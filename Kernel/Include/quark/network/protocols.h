#include <mixins/std/c++types.h>

namespace Quark::System::Net {
    struct IPv4Address
    {
        union
        {
            u8  _bytes[4];
            u16 _words[2];
            u32 _dwords[1];
        };

        IPv4Address(u8 a, u8 b, u8 c, u8 d)
        {
            _bytes[0] = a;
            _bytes[1] = b;
            _bytes[2] = c;
            _bytes[3] = d;
        }

        IPv4Address& operator=(const IPv4Address& other)
        {
            _dwords[0] = other._dwords[0];
            return *this;
        }

        IPv4Address& operator=(const u32& other)
        {
            _dwords[0] = other;
            return *this;
        }

        bool operator==(const IPv4Address& other) const
        {
            return _dwords[0] == other._dwords[0];
        }
    };

    struct IPv6Address
    {
        union
        {
            u8  _bytes[6];
            u16 _words[3];
        };
    };
} // namespace Quark::System::Net