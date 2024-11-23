#include <mixins/io/traits.h>
#include <quark/os/diagnostic/logging.h>

namespace Quark::System::Diagnostic {
    using Qk::StringView;
    using Qk::TextWriter;

    LoggerTextWriter _LoggerWriter;

    // template <typename... TWriters>
    //     requires(Derived<TextWriter, TWriters> && ...)
    // LoggerTextWriter::LoggerTextWriter(Qk::TextWriter* _default)
    // {
    //     add(_default);
    //     m_builder.clear();
    // }

    usize LoggerTextWriter::write(byte b)
    {
        m_builder.append(b);
        return m_builder.len();
    }

    usize LoggerTextWriter::write(Bytes bytes)
    {
        for (auto b : bytes) {
            m_builder.append(b);
        }
        return bytes.len();
    }

    usize LoggerTextWriter::writeStr(StringView str)
    {
        m_builder.append(str);
        return str.len();
    }

    usize LoggerTextWriter::writeRune(Qk::Rune r)
    {
        m_builder.append(r);
        return 1;
    }

    usize LoggerTextWriter::flush()
    {
        StringView view = m_builder.getStringView();
        for (auto* listener : m_listeners) {
            listener->writeStr(view);
            listener->flush();
        }
        m_builder.clear();
        return 0;
    }
}