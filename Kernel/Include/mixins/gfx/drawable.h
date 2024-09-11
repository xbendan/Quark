#include <mixins/gfx/color_format.h>
#include <mixins/math/vec.h>

namespace gfx {
    using Point = Vec<u32, 2>;
    using Size  = Vec<u32, 2>;

    template <ColorFormat TColorFormat>
    class IDrawable
    {
    public:
        virtual void draw(Point, TColorFormat)        = 0;
        virtual void draw(Point, TColorFormat*, Size) = 0;
        virtual void draw(TColorFormat*)              = 0;

        virtual TColorFormat* get() = 0;
    };
}