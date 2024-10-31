#pragma once

#include <mixins/io/stream.h>
#include <mixins/str/string.h>

class TextReader : public IInputStream<String::Unit>
{};

class TextWriter : public IOutputStream<String::Unit>
{
public:
    virtual void dispose() = 0;

    virtual usize Write(StringView::Unit c);
    virtual usize Write(StringView str);
    virtual usize Write(StringView::Unit* str);
    virtual usize Write(Buf<StringView::Unit> const& buf);
    virtual void  WriteNewline() = 0;
};