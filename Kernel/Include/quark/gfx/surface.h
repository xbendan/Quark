#include <mixins/gfx/color_format.h>
#include <quark/gfx/info.h>

namespace Quark::System::Graphics {
    template <typename T, typename TColorFormat>
    concept Drawable = requires(T t, TColorFormat* pColorFormat) {
        requires ColorFormat<TColorFormat>;
        { t.draw(Point(), TColorFormat()) };
        { t.draw(pColorFormat) };
    };

    template <Any<Rgba> TColorFormat>
    struct Surface
    {
        Size          _size;
        TColorFormat* _buffer;

        always_inline constexpr Surface(Size size, TColorFormat* buffer)
            : _size(size)
            , _buffer(buffer)
        {
        }
    };
}