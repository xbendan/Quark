#include <mixins/concurrent/spinlock.h>
#include <mixins/io/traits.h>
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
        , public Qk::TextWriter
    {
        using U = Qk::StringView::Unit;

    public:
        VGATextOutputDevice();
        ~VGATextOutputDevice() = default;

        usize write(byte b) override;
        usize write(Bytes bytes) override;
        usize writeRune(Qk::Rune r) override;
        usize writeStr(Qk::StringView str) override;
        usize flush() override;
        void  newline();
        void  erase();

    private:
        lock_t        m_lock;
        TextOverflow  m_overflow;
        Point         m_cursor;
        volatile u16* m_buffer;
    };
} // namespace Quark::System::Hal::Impls