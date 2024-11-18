#include <mixins/io/scan.h>
#include <mixins/io/traits.h>
#include <mixins/meta/result.h>
#include <mixins/meta/tuple.h>
#include <mixins/meta/vec.h>
#include <mixins/std/c++types.h>
#include <mixins/str/aligned.h>
#include <mixins/str/string_writer.h>
#include <mixins/utils/chars.h>

#include <mixins/std/typeinfo.h>

#include <mixins/std/time.h>
#include <mixins/utils/date.h>

namespace Qk {
    template <typename T>
    struct Formatter;

    struct _Args
    {
        virtual ~_Args()                       = default;
        virtual usize      len() const         = 0;
        virtual Res<usize> format(Scan&       scan,
                                  TextWriter& writer,
                                  usize       index) = 0;
    };

    template <typename... Ts>
    struct Args : public _Args
    {
        Tuple<Ts...> _tuple;

        Args(Ts&&... args)
            : _tuple{ Std::forward<Ts>(args)... }
        {
        }

        usize len() const override { return _tuple.len(); }

        Res<usize> format(Scan& scan, TextWriter& writer, usize index) override
        {
            Res<usize> result =
                Error::InvalidArgument("Format index out of bounds");
            usize i = 0;
            _tuple.visit([&](auto const& t) {
                if (index == i) {
                    using U = Std::RemoveCvRef<decltype(t)>;
                    Formatter<U> formatter;
                    if constexpr (requires() { formatter.parse(scan); }) {
                        formatter.parse(scan);
                    }
                    result = formatter.format(writer, t);
                }
                i++;

                return true;
            });
            return result;
        }
    };

    inline Res<usize> _format(TextWriter& writer,
                              StringView  format,
                              _Args&      args)
    {
        Scan  scan{ format };
        usize len   = 0;
        usize index = 0;

        while (!scan.ended()) {
            Rune c = scan.next();

            if (c == '{') {
                scan.skip(':');
                scan.begin();
                while (scan.peek() != '}') {
                    scan.next();
                }
                scan.next();

                Scan inner{ scan.end() };
                len += args.format(inner, writer, index).unwrap();

                index++;
            } else if (c == '}') {
                return Error::InvalidArgument("Unexpected '}'");
            } else if (c == '\n') {
                writer.writeStr("\r\n"s);
            } else {
                len += writer.writeRune(c);
            }
        }

        return Ok(len);
    }

    inline Res<usize> format(TextWriter& writer, StringView format)
    {
        return Ok(writer.writeStr(format));
    }

    template <typename... Ts>
    inline Res<usize> format(TextWriter& writer, StringView format, Ts&&... ts)
    {
        Args<Ts...> args{ Std::forward<Ts>(ts)... };
        return _format(writer, format, args);
    }

    template <typename T>
    struct Formatter<Aligned<T>>
    {
        Formatter<T> _innerFmt;

        void parse(Scan& scan)
        {
            if constexpr (requires() { _innerFmt.parse(scan); }) {
                _innerFmt.parse(scan);
            }
        }

        Res<usize> format(TextWriter& writer, Aligned<T> val)
        {
            StringWriter<256> buf;
            _innerFmt.format(buf, val._inner);
            usize width  = buf.len();
            usize result = 0;

            if (width < val._width) {
                usize pad = val._width - width;
                switch (val._align) {
                    case Align::LEFT:
                        result += writer.writeStr(buf.getStringView());
                        for (usize i = 0; i < pad; i++)
                            result += writer.writeRune(' ');
                        break;
                    case Align::RIGHT:
                        for (usize i = 0; i < pad; i++)
                            result += writer.writeRune(' ');
                        result += writer.writeStr(buf.getStringView());
                        break;
                    case Align::CENTER:
                        for (usize i = 0; i < pad / 2; i++)
                            result += writer.writeRune(' ');
                        result += writer.writeStr(buf.getStringView());
                        for (usize i = 0; i < pad / 2; i++)
                            result += writer.writeRune(' ');
                        break;
                }
            } else {
                result += writer.writeStr(buf.getStringView());
            }

            return Ok(result);
        }
    };

    struct NumberFormatter
    {
        bool  _prefix{ false };
        usize _base{ 10 };
        usize _width{ 0 };
        bool  _charType{ false };
        bool  _uppercase{ false };
        char  _fill{ ' ' };

        StringView formatPrefix()
        {
            switch (_base) {
                case 2:
                    return "0b"s;
                case 8:
                    return "0o"s;
                case 16:
                    return "0x"s;
                default:
                    return "?-"s;
            }
        }

        void parse(StringView str)
        {
            Scan scan{ str };
            parse(scan);
        }

        void parse(Scan& scan)
        {
            if (scan.skip('#')) {
                _prefix = true;

                Rune c = scan.next();
                if (Chars::IsUpperCase(c)) {
                    c          = Chars::ToLowerCase(c);
                    _uppercase = true;
                }

                switch (c) {
                    case 'b': {
                        _base = 2;
                        break;
                    }

                    case 'o': {
                        _base = 8;
                        break;
                    }

                    case 'd': {
                        _base = 10;
                        break;
                    }

                    case 'x': {
                        _base  = 16;
                        _fill  = '0';
                        _width = sizeof(usize) * 2;
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        template <typename T>
        Res<usize> formatUnsigned(TextWriter& writer, T val)
        {
            auto digit = [this](usize v) {
                if (v < 10) {
                    return '0' + v;
                }
                return (_uppercase ? 'A' : 'a') + (v - 10);
            };

            StackVec<char, 128> buf;

            do {
                buf.pushBack(digit(val % _base));
                val /= _base;
            } while (val != 0 && buf.len() < buf.cap());

            while (_width > buf.len()) {
                buf.pushBack(_fill);
            }
            buf.inverse();

            usize len = 0;
            if (_prefix)
                len += writer.writeStr(formatPrefix());
            len += writer.writeStr(StringView{ buf.buf(), buf.len() });

            return Ok(len);
        }

        template <typename T>
        Res<usize> formatSigned(TextWriter& writer, T val)
        {
            usize len = 0;
            if (val < 0) {
                len += writer.writeRune((Rune)'-');
                val = -val;
            }
            len += formatUnsigned(writer, val).Unwrap();

            return Ok(len);
        }

        Res<usize> formatRune(TextWriter& writer, Rune val)
        {
            if (!_prefix) {
                return Ok(writer.writeRune(val));
            }

            switch (val) {
                case '\n':
                    return Ok(writer.writeStr("\\n"s));
                case '\r':
                    return Ok(writer.writeStr("\\r"s));
                case '\t':
                    return Ok(writer.writeStr("\\t"s));
                case '\0':
                    return Ok(writer.writeStr("\\0"s));
                case '\'':
                    return Ok(writer.writeStr("\\'"s));
                case '\"':
                    return Ok(writer.writeStr("\\\""s));
                case '\\':
                    return Ok(writer.writeStr("\\\\"s));
                case '\?':
                    return Ok(writer.writeStr("\\?"s));
                case '\a':
                    return Ok(writer.writeStr("\\a"s));
                case '\b':
                    return Ok(writer.writeStr("\\b"s));
                case '\f':
                    return Ok(writer.writeStr("\\f"s));
                case '\v':
                    return Ok(writer.writeStr("\\v"s));

                default:
                    return Ok(writer.writeRune(val));
            }
        }
    };

    template <typename T>
        requires(Std::isIntegral<T> and Std::isUnsigned<T>)
    struct Formatter<T> : public NumberFormatter
    {
        Res<usize> format(TextWriter& writer, T const& val)
        {
            if (_charType) {
                return formatRune(writer, val);
            }
            return formatUnsigned(writer, val);
        }
    };

    template <typename T>
        requires(Std::isIntegral<T> and Std::isSigned<T>)
    struct Formatter<T> : public NumberFormatter
    {
        Res<usize> format(TextWriter& writer, T const& val)
        {
            if (_charType) {
                return formatRune(writer, val);
            }
            return formatSigned(writer, val);
        }
    };

    // TODO: Implement floating point formatter
    // template <FloatingPoint T>
    // struct Formatter<T>
    // {
    //     Res<usize> format(TextWriter& writer, T const& val) {}
    // };

    // template <Enum T>
    // struct Formatter<T>
    // {

    // };

    template <typename T>
    struct Formatter<T*>
    {
        bool _prefix{ false };

        void parse(Scan& scan) { _prefix = scan.skip('#'); }

        Res<usize> format(TextWriter& writer, T* val)
        {
            usize len = 0;
            if (_prefix) {
                len += writer.writeRune((Rune)'(');
                len += writer.writeStr(Type::GetName<T>());
                len += writer.writeStr("*)"s);
            }

            if (val) {
                NumberFormatter formatter{
                    ._prefix = true,
                    ._base   = 16,
                    ._width  = sizeof(T*) * 2,
                    ._fill   = '0',
                };
                len += formatter.formatUnsigned(writer, (usize)val).unwrap();
            } else {
                len += writer.writeStr("nullptr");
            }

            return Ok(len);
        }
    };

    template <typename T>
    struct Formatter<T&> : public Formatter<T>
    {};

    template <>
    struct Formatter<nullptr_t>
    {
        Res<usize> format(TextWriter& writer, nullptr_t)
        {
            return Ok(writer.writeStr("nullptr"s));
        }
    };

    template <>
    struct Formatter<bool>
    {
        Res<usize> format(TextWriter& writer, bool val)
        {
            return Ok(writer.writeStr(val ? "True"s : "False"s));
        }
    };

    template <typename T>
    struct Formatter<Ok<T>>
    {
        void parse(Scan& scan)
        {
            if constexpr (requires() { Formatter<T>{}.parse(scan); }) {
                Formatter<T>{}.parse(scan);
            }
        }

        Res<usize> format(TextWriter& writer, Ok<T> const& val)
        {
            if constexpr (SameAs<T, void>)
                return Ok(writer.writeStr("Ok()"s));
            else
                return Formatter<T>{}.format(writer, val.inner);
        }
    };

    template <>
    struct Formatter<Error>
    {
        Res<usize> format(TextWriter& writer, Error const& val)
        {
            return Ok(writer.writeStr(StringView{ val.message() }));
        }
    };

    template <typename T, typename E>
    struct Formatter<Res<T, E>>
    {
        Formatter<T> _ok;
        Formatter<E> _err;

        void parse(Scan& scan)
        {
            if constexpr (requires() { _ok.parse(scan); }) {
                _ok.parse(scan);
            }

            if constexpr (requires() { _err.parse(scan); }) {
                _err.parse(scan);
            }
        }

        Res<usize> format(TextWriter& writer, Res<T> const& val)
        {
            if (val)
                return _ok.format(writer, val.unwrap());
            else
                return _err.format(writer, val.Err());
        }
    };

    template <EncodingFormat E>
    struct StringFormatter
    {
        bool _prefix{ false };

        void parse(Scan& scan)
        {
            if (scan.skip('#'))
                _prefix = true;
        }

        Res<usize> format(TextWriter& writer, _StringView<E> text)
        {
            if (!_prefix)
                return Ok(writer.writeStr(text));

            usize len = 0;
            len += writer.writeRune('"');
            for (Rune c : iterRunes(text)) {
                switch (c) {
                        // clang-format off
                    case '\n': len += writer.writeStr("\\n"s); break;
                    case '\r': len += writer.writeStr("\\r"s); break;
                    case '\t': len += writer.writeStr("\\t"s); break;
                    case '\0': len += writer.writeStr("\\0"s); break;
                    case '\'': len += writer.writeStr("\\'"s); break;
                    case '\"': len += writer.writeStr("\\\""s); break;
                    case '\\': len += writer.writeStr("\\\\"s); break;
                    case '\?': len += writer.writeStr("\\?"s); break;
                    case '\a': len += writer.writeStr("\\a"s); break;
                    case '\b': len += writer.writeStr("\\b"s); break;
                    case '\f': len += writer.writeStr("\\f"s); break;
                    case '\v': len += writer.writeStr("\\v"s); break;
                    default: len += writer.writeRune(c); break;
                        // clang-format on
                }
            }
            len += writer.writeRune('"');

            return Ok(len);
        }
    };

    template <EncodingFormat E>
    struct Formatter<_StringView<E>> : public StringFormatter<E>
    {};

    template <EncodingFormat E>
    struct Formatter<_String<E>> : public StringFormatter<E>
    {
        Res<usize> format(TextWriter& writer, _String<E> const& val)
        {
            return StringFormatter<E>::format(writer, val.view());
        }
    };

    template <>
    struct Formatter<const char*> : public StringFormatter<Encoding::UTF8>
    {
        Res<usize> format(TextWriter& writer, const char* val)
        {
            StringView sv(val, strnlen(val, 512));
            return StringFormatter<Encoding::UTF8>::format(writer, sv);
        }
    };
}