#include <mixins/io/text.h>
#include <mixins/meta/buf.h>
#include <quark/gfx/display.h>

namespace VESA {
    using namespace Quark::System::Graphics;

    class VGATextOutputDevice
        : public IVideoDisplayDevice
        , public Io::TextWriter
    {
    public:
        VGATextOutputDevice();
        ~VGATextOutputDevice();

        void writeStr(string str) override;
        void writeStr(String<Ascii> str);
        void writeNewline() override;

    private:
        Size  m_resolution{ 80, 25 };
        Point m_cursor{ 0, 0 };
        u16*  m_buffer{ (u16*)0xb8000 };
    };
} // namespace Quark::System::Hal::Impls