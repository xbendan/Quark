#include <mixins/std/c++types.h>

namespace Quark::System::Platform::X64 {
    struct FxState
    {
        u16 _fcontrol;
        u16 _fstatus;
        u8  _ftag;
        u8  __reserved__1; // Just a zero
        u16 _fop;
        u64 _rip;
        u64 _rdp;
        u32 _mxcsr;       // SSE control/status register
        u32 _mxcsrMask;   // SSE control/status mask
        u8  _st[8][16];   // FPU registers
        u8  _xmm[16][16]; // XMM registers
    };
}