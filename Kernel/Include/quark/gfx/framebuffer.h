#pragma once

#include <quark/gfx/display.h>
#include <quark/gfx/surface.h>

namespace Quark::System::Graphics {
    class LinearFramebufferDevice : public IVideoOutputDevice
    {
    public:
        LinearFramebufferDevice(u64 address,
                                u32 width,
                                u32 height,
                                u16 pitch,
                                u16 bpp)
            : IVideoOutputDevice("Linear Framebuffer", { width, height })
            , _surface({ width, height }, reinterpret_cast<Rgba*>(address))
        {
        }
        ~LinearFramebufferDevice() = default;

    private:
        Surface<Rgba> _surface;
    };
}