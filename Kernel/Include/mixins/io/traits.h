#pragma once

#include <mixins/io/types.h>
#include <mixins/meta/slice.h>
#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/str/encode.h>
#include <mixins/str/string_view.h>

namespace Qk {
    template <typename TClass>
    concept Writable = requires(TClass& t, byte b, Bytes bytes) {
        { t.write(b) } -> SameAs<usize>;
        { t.write(bytes) } -> SameAs<usize>;
    };

    template <typename TClass>
    concept Readable = requires(TClass& t, byte& b, Bytes bytes) {
        { t.read(b) } -> SameAs<usize>;
        { t.read(bytes) } -> SameAs<usize>;
    };

    template <typename TClass>
    concept Seekable = requires(TClass& t, SeekPos pos) {
        { t.seek(pos) } -> SameAs<usize>;
    };

    template <typename TClass>
    concept Flushable = requires(TClass& t) {
        { t.flush() } -> SameAs<usize>;
    };

    template <typename TClass>
    concept Duplexable = Writable<TClass> and Readable<TClass>;

    template <typename TClass>
    concept SeekableWritable = Seekable<TClass> and Writable<TClass>;

    template <typename TClass>
    concept SeekableReadable = Seekable<TClass> and Readable<TClass>;

    template <typename TClass>
    concept SeekableDuplexable = Seekable<TClass> and Duplexable<TClass>;

    template <typename TClass>
    concept Stream = Writable<TClass> and Readable<TClass> and
                     Seekable<TClass> and Flushable<TClass>;

    struct Writer
    {
        virtual ~Writer() = default;

        virtual usize write(byte b)      = 0;
        virtual usize write(Bytes bytes) = 0;
    };

    struct Reader
    {
        virtual ~Reader() = default;

        virtual usize read(byte& b)     = 0;
        virtual usize read(Bytes bytes) = 0;
    };

    struct Seeker
    {
        virtual ~Seeker() = default;

        virtual usize seek(SeekPos pos) = 0;
    };

    struct Flusher
    {
        virtual ~Flusher() = default;

        virtual usize flush() = 0;
    };

    struct TextWriter
        : public Writer
        , public Flusher
    {
        using Writer::write;

        template <EncodingFormat E>
        usize writeStr(_StringView<E> str)
        {
            usize len = 0;
            for (auto r : iterRunes(str)) {
                len += writeStr(r);
            }
            return len;
        }

        virtual usize writeStr(StringView str) = 0;

        virtual usize writeRune(Rune r) = 0;

        virtual usize flush() override { return 0uz; }
    };

    struct TextReader
        : public Reader
        , public Seeker
    {
        using Reader::read;

        template <EncodingFormat E>
        usize read(_StringView<E> str)
        {
            usize len = 0;
            for (Rune& r : str) {
                len += read(r);
            }
            return len;
        }

        virtual usize read(Rune& r) = 0;

        usize seek(SeekPos) override { return 0uz; }
    };

};