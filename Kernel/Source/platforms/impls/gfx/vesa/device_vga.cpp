#include <drivers/gfx/vesa/device_vga.h>
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

    void VGATextOutputDevice::Write(char c)
    {
        ScopedLock lock(m_lock);
        if (c == '\n') {
            WriteNewline();
            return;
        }

        m_buffer[m_cursor.y * VGA_TEXT_WIDTH + m_cursor.x] =
            (u16)(TextColor::TEXT_COLOR_WHITE << 8) | c;
        m_cursor.x++;
        if (m_cursor.x >= VGA_TEXT_WIDTH)
            WriteNewline();
    }

    void VGATextOutputDevice::Write(string str)
    {
        ScopedLock lock(m_lock);
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
        }
    }

    void VGATextOutputDevice::WriteAscii(String<Ascii> str)
    {
        Write((string)str);
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

    void VGATextOutputDevice::operator<<(string str)
    {
        Write(str);
    }

    void VGATextOutputDevice::operator<<(u8 c)
    {
        Write((char)c);
    }

    void VGATextOutputDevice::operator<<(u8* str)
    {
        Write(string(str));
    }

    void VGATextOutputDevice::operator<<(Buf<u8> const& buf)
    {
        Write(string(buf.Data(), buf.Length()));
    }
}