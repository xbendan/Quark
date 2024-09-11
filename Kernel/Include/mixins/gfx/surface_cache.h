#include <mixins/gfx/color_format.h>
#include <mixins/gfx/drawable.h>

namespace gfx {
    template <ColorFormat TColorFormat>
    class Surface : public IDrawable<TColorFormat>
    {
    public:
        virtual void draw(Point, TColorFormat) override;
        virtual void draw(Point, TColorFormat*, Size) override;
        virtual void draw(TColorFormat*) override;

        virtual TColorFormat* get() override;

    private:
        Size          _size;
        TColorFormat* _buffer;
    };

    template <ColorFormat TColorFormat>
    class DoubleBufferingSurface : public IDrawable<TColorFormat>
    {
    public:
        DoubleBufferingSurface(Size size, TColorFormat* buffer)
            : _size(size)
            , _buffer(buffer)
            , _writable(new TColorFormat[size.x * size.y])
        {
        }

        virtual void draw(Point, TColorFormat) override;
        virtual void draw(Point, TColorFormat*, Size) override;
        virtual void draw(TColorFormat*) override;

        virtual TColorFormat* get() override;

    private:
        Size          _size;
        TColorFormat *_buffer, _writable;
    };
}
