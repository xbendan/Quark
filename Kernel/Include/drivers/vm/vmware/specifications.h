#include <mixins/std/c++types.h>

namespace VmWare {
    static constexpr u32 MAGIC  = 0x564D5868;
    static constexpr u32 PORT   = 0x5658;
    static constexpr u32 PORTHB = 0x5659;

    struct Command
    {
        union
        {
            u32 _ax;
            u32 _magic;
        };
        union
        {
            u32 _bx;
            u32 _size;
        };
        union
        {
            u32 _cx;
            u32 _command;
        };
        union
        {
            u32 _dx;
            u32 _port;
        };
        u32 _si;
        u32 _di;
    };
}