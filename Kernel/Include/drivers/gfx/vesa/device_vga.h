#include <mixins/concurrent/spinlock.h>
#include <mixins/io/text.h>
#include <mixins/meta/buf.h>
#include <quark/gfx/display.h>

#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25
#define VGA_TEXT_BUFFER ((u16*)0xb8000)

namespace VESA {
    using namespace Quark::System::Graphics;

    enum TextColor
    {
        TEXT_COLOR_BLACK         = 0,
        TEXT_COLOR_BLUE          = 1,
        TEXT_COLOR_GREEN         = 2,
        TEXT_COLOR_CYAN          = 3,
        TEXT_COLOR_RED           = 4,
        TEXT_COLOR_MAGENTA       = 5,
        TEXT_COLOR_BROWN         = 6,
        TEXT_COLOR_LIGHT_GREY    = 7,
        TEXT_COLOR_DARK_GREY     = 8,
        TEXT_COLOR_LIGHT_BLUE    = 9,
        TEXT_COLOR_LIGHT_GREEN   = 10,
        TEXT_COLOR_LIGHT_CYAN    = 11,
        TEXT_COLOR_LIGHT_RED     = 12,
        TEXT_COLOR_LIGHT_MAGENTA = 13,
        TEXT_COLOR_LIGHT_BROWN   = 14,
        TEXT_COLOR_WHITE         = 15,
    };

    enum class TextOverflow
    {
        Wrap,
        Scroll,
    };

    class VGATextOutputDevice
        : public IVideoOutputDevice
        , public Io::TextWriter
    {
    public:
        VGATextOutputDevice();
        ~VGATextOutputDevice();

        void Write(char c) override;
        void Write(string str) override;
        void WriteAscii(String<Ascii> str);
        void WriteNewline() override;
        void ClearScreen();

        void operator<<(string str) override;
        void operator<<(u8 c) override;
        void operator<<(u8* str) override;
        void operator<<(Buf<u8> const& buf) override;

    private:
        lock_t        m_lock;
        TextOverflow  m_overflow;
        Point         m_cursor;
        volatile u16* m_buffer;
    };
} // namespace Quark::System::Hal::Impls