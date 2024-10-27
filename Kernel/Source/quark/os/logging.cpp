#include <mixins/fmt/translator.h>
#include <mixins/io/text.h>
#include <mixins/utils/array_list.h>
#include <quark/os/logging.h>

namespace Quark::System::Diagnostic {
    ArrayList<Io::TextWriter*> logWriters;

    void log(LogLevel level, string msg) {}
    void log(LogLevel level, string msg, fmt::_Args& args) {}

    static_assert(fmt::Translatable<const char*>);
}