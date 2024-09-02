#include <siberix/gfx/device.h>
#include <stdcxx/buffer.h>

namespace Quark::Video {
    using namespace Gfx;

    class FramebufferDisplay : public LocalDisplay
    {
    public:
        FramebufferDisplay(u32 width, u32 height)
            : LocalDisplay("Framebuffer Buffered Display", width, height)
        {
        }

    private:
        Std::Buffer<Pixel<8>> m_buf;
    };
} // namespace Quark::Video