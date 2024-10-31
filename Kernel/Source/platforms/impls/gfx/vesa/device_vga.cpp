#include <drivers/gfx/vesa/device_vga.h>
#include <mixins/meta/result.h>
#include <quark/memory/address_range.h>

namespace VESA {
    using Quark::System::Memory::AddressRange;

    VGATextOutputDevice::VGATextOutputDevice()
        : IVideoOutputDevice{ "VGA Text Output", { 80, 25 } }
        , m_overflow(TextOverflow::Scroll)
        , m_cursor(0, 0)
        , m_buffer(VGA_TEXT_BUFFER)
    {
        ClearScreen();
    }

    usize VGATextOutputDevice::Write(U c)
    {
        ScopedLock lock(m_lock);
        if (c == '\n') {
            WriteNewline();
        }

        m_buffer[m_cursor.y * VGA_TEXT_WIDTH + m_cursor.x] =
            (u16)(TextColor::TEXT_COLOR_WHITE << 8) | c;
        m_cursor.x++;
        if (m_cursor.x >= VGA_TEXT_WIDTH)
            WriteNewline();

        return 1;
    }

    usize VGATextOutputDevice::Write(StringView str)
    {
        ScopedLock lock(m_lock);
        usize      len = 0;
        for (char c : str) {
            if (c == '\n') {
                WriteNewline();
                continue;
            }

            m_buffer[m_cursor.y * VGA_TEXT_WIDTH + m_cursor.x] =
                (u16)(TextColor::TEXT_COLOR_WHITE << 8) | c;
            m_cursor.x++;
            if (m_cursor.x >= VGA_TEXT_WIDTH)
                WriteNewline();

            len++;
        }

        return len;
    }

    usize VGATextOutputDevice::Write(U* up)
    {
        return Write(StringView(up));
    }

    usize VGATextOutputDevice::Write(Buf<U> const& buf)
    {
        return Write(StringView(buf.buf(), buf.len()));
    }

    void VGATextOutputDevice::WriteAscii(_StringView<Ascii> str)
    {
        Write(StringView(str.buf()));
    }

    void VGATextOutputDevice::WriteNewline()
    {
        m_cursor.x = 0;
        m_cursor.y++;

        if (m_cursor.y >= VGA_TEXT_HEIGHT) {
            if (m_overflow == TextOverflow::Scroll) {
                m_cursor.y = VGA_TEXT_HEIGHT - 1;

                AddressRange fromRange(m_buffer + VGA_TEXT_WIDTH,
                                       VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1) *
                                           sizeof(u16));
                fromRange.CopyTo((u8*)(m_buffer));

                for (u32 x = 0; x < VGA_TEXT_WIDTH; x++) {
                    m_buffer[(VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH + x] = 0;
                }
            } else {
                m_cursor.y = 0;
                ClearScreen();
            }
        }
    }

    void VGATextOutputDevice::ClearScreen()
    {
        ScopedLock lock(m_lock);
        for (u32 i = 0; i < VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT; i++)
            m_buffer[i] = 0;
    }
}