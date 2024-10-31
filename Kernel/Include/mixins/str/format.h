#include <mixins/io/scan.h>
#include <mixins/io/text.h>
#include <mixins/meta/result.h>
#include <mixins/meta/tuple.h>
#include <mixins/meta/vec.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/chars.h>

#include <mixins/std/typeinfo.h>

#include <mixins/std/time.h>
#include <mixins/utils/date.h>

template <typename T>
struct Formatter;

struct _Args
{
    virtual ~_Args()               = default;
    virtual usize      len() const = 0;
    virtual Res<usize> format(Scan& scan, TextWriter& writer, usize index) = 0;
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

inline Res<usize>
_format(TextWriter& writer, StringView format, _Args& args)
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
            len += args.format(inner, writer, index).Unwrap();

            index++;
        } else if (c == '}') {
            return Error::InvalidArgument("Unexpected '}'");
        } else if (c == '\n') {
            writer.WriteNewline();
        } else {
            len += writer.Write(c);
        }
    }

    return Ok(len);
}

inline Res<usize>
format(TextWriter& writer, StringView format)
{
    return Ok(writer.Write(format));
}

template <typename... Ts>
inline Res<usize>
format(TextWriter& writer, StringView format, Ts&&... ts)
{
    Args<Ts...> args{ Std::forward<Ts>(ts)... };
    return _format(writer, format, args);
}

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
                return "0b";
            case 8:
                return "0o";
            case 16:
                return "0x";
            default:
                return "";
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
        requires(Std::isIntegral<T> and Std::isUnsigned<T>)
    Res<usize> formatUnsigned(TextWriter& writer, T val)
    {
        auto digit = [](usize v) {
            if (v < 10) {
                return '0' + v;
            }
            return 'a' + (v - 10);
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
            len += writer.Write(formatPrefix());
        len += writer.Write(StringView{ buf.buf(), buf.len() });

        return Ok(len);
    }

    template <typename T>
        requires(Std::isIntegral<T> and Std::isSigned<T>)
    Res<usize> formatSigned(TextWriter& writer, T val)
    {
        usize len = 0;
        if (val < 0) {
            len += writer.Write('-');
            val = -val;
        }
        len += formatUnsigned(writer, val).Unwrap();

        return Ok(len);
    }

    Res<usize> formatRune(TextWriter& writer, Rune val)
    {
        if (!_prefix) {
            return Ok(writer.Write(val));
        }

        switch (val) {
            case '\n':
                return Ok(writer.Write("\\n"s));
            case '\r':
                return Ok(writer.Write("\\r"s));
            case '\t':
                return Ok(writer.Write("\\t"s));
            case '\0':
                return Ok(writer.Write("\\0"s));
            case '\'':
                return Ok(writer.Write("\\'"s));
            case '\"':
                return Ok(writer.Write("\\\""s));
            case '\\':
                return Ok(writer.Write("\\\\"s));
            case '\?':
                return Ok(writer.Write("\\?"s));
            case '\a':
                return Ok(writer.Write("\\a"s));
            case '\b':
                return Ok(writer.Write("\\b"s));
            case '\f':
                return Ok(writer.Write("\\f"s));
            case '\v':
                return Ok(writer.Write("\\v"s));

            default:
                return Ok(writer.Write(val));
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

template <FloatingPoint T>
struct Formatter<T>
{
    Res<usize> format(TextWriter& writer, T const& val) {}
};

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
            len += writer.Write('(');
            len += writer.Write(Type::GetName<T>());
            len += writer.Write("*)");
        }

        if (val) {
            NumberFormatter formatter{
                ._prefix = true,
                ._base   = 16,
                ._width  = sizeof(T*) * 2,
                ._fill   = '0',
            };
            len += formatter.formatUnsigned(writer, (usize)val).Unwrap();
        } else {
            len += writer.Write("nullptr");
        }

        return Ok(len);
    }
};

template <>
struct Formatter<nullptr_t>
{
    Res<usize> format(TextWriter& writer, nullptr_t)
    {
        return Ok(writer.Write("nullptr"s));
    }
};

template <>
struct Formatter<bool>
{
    Res<usize> format(TextWriter& writer, bool val)
    {
        return Ok(writer.Write(val ? "True"s : "False"s));
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
            return Ok(writer.Write("Ok()"s));
        else
            return Formatter<T>{}.format(writer, val.inner);
    }
};

template <>
struct Formatter<Error>
{
    Res<usize> format(TextWriter& writer, Error const& val)
    {
        return Ok(writer.Write(StringView{ val.message() }));
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
            return _ok.format(writer, val.Unwrap());
        else
            return _err.format(writer, val.Err());
    }
};

template <EncodingFormat E>
struct StringFormatter
{};

template <EncodingFormat E>
struct Formatter<_StringView<E>> : public StringFormatter<E>
{};

template <>
struct Formatter<const char*> : public StringFormatter<Utf8>
{};