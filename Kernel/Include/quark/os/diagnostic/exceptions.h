#include <mixins/meta/exception.h>

namespace System::Diagnostics {
    using Qk::Exception;

    struct IllegalStateException : public Exception<>
    {};
}