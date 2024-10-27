#pragma once

#include <mixins/fmt/args.h>
#include <mixins/fmt/translator.h>
#include <mixins/io/text.h>
#include <mixins/std/panic.h>
#include <mixins/std/string.h>
#include <mixins/utils/flags.h>

namespace Quark::System::Diagnostic {
    enum class LogLevel
    {
        Debug = (1 << 0),
        Info  = (1 << 1),
        Warn  = (1 << 2),
        Error = (1 << 3),
        Fatal = (1 << 4),
    };
    MakeFlags$(LogLevel);

    static constexpr Flags<LogLevel> AllLevels =
        (LogLevel::Debug | LogLevel::Info | LogLevel::Warn | LogLevel::Error |
         LogLevel::Fatal);

    class Logger
    {
    public:
        static inline void AddListener(Flags<LogLevel> levels,
                                       TextWriter*     textOutput);
    };

    void log(LogLevel level, string msg);
    void log(LogLevel level, string msg, fmt::_Args& args);

    inline void log(string msg, LogLevel level = LogLevel::Info)
    {
        log(level, msg);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void log(LogLevel level, string msg, TArgs... args)
    {
        fmt::Args<TArgs...> fmtArgs(Std::forward<TArgs>(args)...);
        log(level, msg, fmtArgs);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void debug(string msg, TArgs... args)
    {
        log(LogLevel::Debug, msg, args...);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void info(string msg, TArgs... args)
    {
        log(LogLevel::Info, msg, args...);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void warn(string msg, TArgs... args)
    {
        log(LogLevel::Warn, msg, args...);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void error(string msg, TArgs... args)
    {
        log(LogLevel::Error, msg, args...);
    }

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void fatal(string msg, TArgs... args)
    {
        fmt::Args<TArgs...> fmtArgs(Std::forward<TArgs>(args)...);
        log(LogLevel::Fatal, msg, fmtArgs);

        Std::SystemPanic(msg);
    }
}

#if defined(DEBUG_MODE)
using namespace Quark::System::Diagnostic;
#endif