#include <mixins/io/stream.h>
#include <mixins/std/string.h>

namespace Io {
    class TextReader : public IInputStream<string::Unit>
    {};

    class TextWriter : public IOutputStream<string::Unit>
    {
    public:
        virtual void Write(string str) = 0;
        virtual void WriteNewline()    = 0;
    };
}