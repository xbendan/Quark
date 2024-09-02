#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/string.h>

namespace fmt {
    template <typename CharType>
    class StringBufferBase;

    template <typename CharType, usize BufferSize>
    class StringBuffer
    {
    public:
        void append(CharType const* str, usize len)
        {
            for (usize i = 0; i < len; ++i)
                append(str[i]);
        }

        void append(CharType c)
        {
            if (m_p < BufferSize)
                m_buffer[m_p++] = c;
        }

        void append(StringBuffer& buffer)
        {
            append(buffer.data(), buffer.size());
        }

        CharType const* data() const { return m_buffer; }
        usize           size() const { return BufferSize; }
        usize           len() const { return m_p; }

    private:
        CharType m_buffer[BufferSize];
        usize    m_p = 0;
    };
}