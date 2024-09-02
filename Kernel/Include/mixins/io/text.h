#include <mixins/io/stream.h>
#include <mixins/std/string.h>

namespace Io {
    class TextReader : public IInputStream<String<>::Unit>
    {};

    class TextWriter : public IOutputStream<String<>::Unit>
    {
    public:
        virtual void writeStr(string str) = 0;
        virtual void writeNewline()       = 0;
    };
}