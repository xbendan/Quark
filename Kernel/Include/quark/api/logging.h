#include <mixins/fmt/args.h>
#include <mixins/fmt/translator.h>
#include <mixins/std/string.h>

namespace Quark::System::API {
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    void log(string msg);

    void log(string msg, fmt::_Args& args);

    // template <typename... Ts>
    //     requires(fmt::Translatable<Ts> && ...)
    // void log(string msg, LogLevel level = LogLevel::Info, Ts...);

    template <typename... TArgs>
        requires(fmt::Translatable<Std::RemoveRef<TArgs>> && ...)
    inline void log(string msg, TArgs... args)
    {
        fmt::Args<TArgs...> fmtArgs(Std::forward<TArgs>(args)...);
        log(msg, fmtArgs);
    }
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::System::API;
#endif