#include <mixins/fmt/translator.h>
#include <mixins/io/text.h>
#include <mixins/utils/array_list.h>
#include <quark/api/logging.h>

namespace Quark::System::API {
    ArrayList<Io::TextWriter*> logWriters;

    void log(String<> msg) {}

    void log(String<> msg, fmt::_Args& args) {}

    static_assert(fmt::Translatable<const char*>);
}