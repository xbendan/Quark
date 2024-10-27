#pragma once

#include <mixins/io/stream.h>
#include <mixins/std/string.h>

class TextReader : public IInputStream<string::Unit>
{};

class TextWriter : public IOutputStream<string::Unit>
{
public:
    virtual void Write(char c)     = 0;
    virtual void Write(string str) = 0;
    virtual void WriteNewline()    = 0;

    virtual void operator<<(string str)         = 0;
    virtual void operator<<(u8 c)               = 0;
    virtual void operator<<(u8* str)            = 0;
    virtual void operator<<(Buf<u8> const& buf) = 0;
};