#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>

template <typename TSource, class U = u8>
concept InputStream = requires(TSource& t, U* buf, usize len) {
    { t.in() } -> SameAs<U>;
    { t.in(buf, len) } -> ConvertibleTo<bool>;
    { t.skip(len) };
    { t.close() };
};

template <typename TSource, class U = u8>
concept OutputStream = requires(TSource& t, U u, U* buf, usize len) {
    { t.out(u) } -> SameAs<void>;
    { t.out(buf, len) } -> ConvertibleTo<bool>;
    { t.skip(len) };
    { t.close() };
};

template <class U = u8>
class IInputStream
{
public:
    virtual U    in()                  = 0;
    virtual bool in(U* buf, usize len) = 0;
    virtual void skip(usize len)       = 0;
    virtual void close()               = 0;
};
static_assert(InputStream<IInputStream<>>);

template <class U = u8>
class IOutputStream
{
public:
    virtual void out(U u)               = 0;
    virtual bool out(U* buf, usize len) = 0;
    virtual void skip(usize len)        = 0;
    virtual void close()                = 0;
};
static_assert(OutputStream<IOutputStream<>>);
