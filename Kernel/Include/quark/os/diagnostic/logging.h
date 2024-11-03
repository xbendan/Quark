#pragma once

#include <mixins/io/traits.h>
#include <mixins/meta/result.h>
#include <mixins/meta/vec.h>
#include <mixins/std/panic.h>
#include <mixins/str/format.h>
#include <mixins/str/string.h>
#include <mixins/str/string_builder.h>
#include <mixins/utils/flags.h>
#include <mixins/utils/strings.h>
#include <quark/os/diagnostic/level.h>

namespace Quark::System::Diagnostic {

    struct Logger
    {
        Level       Lv;
        const char* Name;
    };

    struct LoggerTextWriter : Qk::TextWriter
    {
        LoggerTextWriter() = default;
        LoggerTextWriter(Qk::TextWriter* _default);

        usize write(byte b) override;
        usize write(Bytes bytes) override;
        usize writeStr(Qk::StringView str) override;
        usize writeRune(Qk::Rune r) override;
        usize flush() override;

        usize writeLine(Logger log, Qk::StringView str)
        {
            Qk::format(*this,
                       "[ {} ] {}"s,
                       Qk::align(log.Name, Qk::Align::LEFT, 6),
                       str);
            if (not Qk::Strings::EndsWith(str, '\n'))
                writeStr("\r\n"s);

            return flush();
        }

        template <typename... TArgs>
        usize writeLine(Logger log, Qk::StringView format, TArgs&&... args)
        {
            Qk::format(
                *this, "[ {} ] "s, Qk::align(log.Name, Qk::Align::LEFT, 6));
            Qk::format(*this, format, args...);
            if (not Qk::Strings::EndsWith(format, '\n'))
                writeStr("\r\n"s);

            return flush();
        }

        void add(Qk::TextWriter* listener) { m_listeners.pushBack(listener); }

    private:
        Qk::StackVec<Qk::TextWriter*, 16> m_listeners;
        Qk::StringBuilder<512>            m_builder;
    };

    extern LoggerTextWriter _LoggerWriter;

    static constexpr Logger _Info{ Level::Info, "INFO" };
    static constexpr Logger _Warn{ Level::Warning, "WARN" };
    static constexpr Logger _Error{ Level::Error, "ERROR" };
    static constexpr Logger _Critical{ Level::Critical, "FATAL" };

    template <typename... TArgs>
    void log$(Qk::StringView format, TArgs... args)
    {
        _LoggerWriter.writeLine(_Info, format, args...);
    }

    template <typename... TArgs>
    void info$(Qk::StringView format, TArgs... args)
    {
        _LoggerWriter.writeLine(_Info, format, args...);
    }

    template <typename... TArgs>
    void warn$(Qk::StringView format, TArgs... args)
    {
        _LoggerWriter.writeLine(_Warn, format, args...);
    }

    template <typename... TArgs>
    void error$(Qk::StringView format, TArgs... args)
    {
        _LoggerWriter.writeLine(_Error, format, args...);
    }

    template <typename... TArgs>
    void crit$(Qk::StringView format, TArgs... args)
    {
        _LoggerWriter.writeLine(_Critical, format, args...);
    }

#define log(message) _LoggerWriter.writeLine(_Info, message)
#define info(message) _LoggerWriter.writeLine(_Info, message)
#define warn(message) _LoggerWriter.writeLine(_Warn, message)
#define error(message) _LoggerWriter.writeLine(_Error, message)
#define crit(message) _LoggerWriter.writeLine(_Critical, message)

    // void Log$(Qk::StringView message);

    // template <typename... TArgs>
    // void Log$(Qk::StringView format, TArgs... args);
}

#if defined(DEBUG_MODE)
using namespace Quark::System::Diagnostic;
#endif