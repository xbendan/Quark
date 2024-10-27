#pragma once

#include <mixins/meta/buf.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>

template <typename TSource, class U = u8>
concept InputStream = requires(TSource& t, U& u, usize len, Buf<U> const& buf) {
    { t() } -> SameAs<U>;
    { t >> u } -> SameAs<void>;
    { t >> buf } -> SameAs<void>;
    { t.operator++() } -> SameAs<void>;
    { t += len } -> SameAs<void>;

    { t.Dispose() };
};

template <typename TSource, class U = u8>
concept OutputStream = requires(TSource& t, U u, U* up, Buf<U> const& buf) {
    { t << u } -> SameAs<void>;
    { t << buf } -> SameAs<void>;
    { t << up } -> SameAs<void>;

    { t.Dispose() };
};

template <class U = u8>
class IInputStream
{
public:
    virtual void Dispose() = 0;

    virtual U    operator()()                  = 0;
    virtual void operator>>(U& u)              = 0;
    virtual void operator>>(Buf<U> const& buf) = 0;
    virtual void operator++()                  = 0;
    virtual void operator+=(usize len)         = 0;
};
static_assert(InputStream<IInputStream<>>);

template <class U = u8>
class IOutputStream
{
public:
    virtual void Dispose() = 0;

    virtual void operator<<(U u)               = 0;
    virtual void operator<<(U* up)             = 0;
    virtual void operator<<(Buf<U> const& buf) = 0;
};
static_assert(OutputStream<IOutputStream<>>);

class BufferInputStream : public IInputStream<u8>
{
public:
    BufferInputStream(u8* buffer, usize size)
        : m_buffer(buffer)
        , m_index(0)
        , m_size(size)
    {
    }

    u8 operator()() override { return m_buffer[m_index]; }

    void operator>>(u8& u) override { u = m_buffer[m_index++]; }

    void operator>>(Buf<u8> const& buf) override
    {
        Buf<u8>& _buf = const_cast<Buf<u8>&>(buf);
        for (usize i = 0; i < buf.Length(); i++) {
            _buf[i] = m_buffer[m_index++];
        }
    }

    void operator++() override { m_index++; }

    void operator+=(usize len) override { m_index += len; }

    void Dispose() override {}

private:
    u8*   m_buffer;
    usize m_index;
    usize m_size;
};

class BufferOutputStream : public IOutputStream<u8>
{
public:
    BufferOutputStream(u8* buffer, usize size)
        : m_buffer(buffer)
        , m_index(0)
        , m_size(size)
    {
    }

    void operator<<(u8 u) override { m_buffer[m_index++] = u; }

    void operator<<(u8* up) override
    {
        while (*up && m_index < m_size) {
            m_buffer[m_index++] = *up++;
        }
    }

    void operator<<(Buf<u8> const& buf) override
    {
        for (usize i = 0; i < buf.Length(); i++) {
            m_buffer[m_index++] = buf.Data()[i];
        }
    }

    void Dispose() override {}

private:
    u8*   m_buffer;
    usize m_index;
    usize m_size;
};