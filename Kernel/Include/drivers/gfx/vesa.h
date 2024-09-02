#include <mixins/io/text.h>
#include <quark/gfx/display.h>

namespace VESA {
    using namespace Quark::Graphics;

    class VGATextOutputDevice
        : public IVideoOutputDevice
        , public Io::TextWriter
    {
    public:
        VGATextOutputDevice();
        ~VGATextOutputDevice();

        void writeStr(string str) override;
        void writeStr(String<Ascii> str);
        void writeNewline() override;

    private:
        Point m_cursor{ 0, 0 };
        u16*  m_buffer{ (u16*)0xb8000 };
    };
} // namespace Quark::Hal::Impls