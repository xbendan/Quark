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
        erase();
    }

    usize VGATextOutputDevice::write(byte b)
    {
        ScopedLock lock(m_lock);
        if (b == '\n')
            newline();

        m_buffer[m_cursor.y * VGA_TEXT_WIDTH + m_cursor.x] =
            (u16)(TextColor::TEXT_COLOR_WHITE << 8) | b;
        m_cursor.x++;
        if (m_cursor.x >= VGA_TEXT_WIDTH)
            newline();

        return 1;
    }

    usize VGATextOutputDevice::write(Bytes bytes)
    {
        Qk::StringView str{ (const char*)bytes.buf(), bytes.len() };
        return writeStr(str);
    }

    usize VGATextOutputDevice::writeStr(Qk::StringView str)
    {
        ScopedLock lock(m_lock);
        usize      len = 0;
        for (char c : str) {
            if (c == '\n') {
                newline();
                continue;
            }

            m_buffer[m_cursor.y * VGA_TEXT_WIDTH + m_cursor.x] =
                (u16)(TextColor::TEXT_COLOR_WHITE << 8) | c;
            m_cursor.x++;
            if (m_cursor.x >= VGA_TEXT_WIDTH)
                newline();

            len++;
        }

        return len;
    }

    usize VGATextOutputDevice::writeRune(Qk::Rune r)
    {
        return write((byte)r);
    }

    usize VGATextOutputDevice::flush()
    {
        return 0;
    }

    void VGATextOutputDevice::newline()
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
                erase();
            }
        }
    }

    void VGATextOutputDevice::erase()
    {
        memset(
            (void*)m_buffer, 0, VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * sizeof(u16));
    }
}