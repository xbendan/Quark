#include <mixins/io/traits.h>
#include <mixins/meta/result.h>
#include <mixins/str/encode.h>
#include <mixins/str/string_builder.h>

namespace Qk {
    template <EncodingFormat E, usize TLen = 0>
    struct _StringWriter
        : public TextWriter
        , public _StringBuilder<E, TLen>
    {
        _StringWriter()
            requires(TLen > 0)
            : _StringBuilder<E, TLen>()
        {
        }

        _StringWriter(usize cap = 16)
            requires(TLen == 0)
            : _StringBuilder<E, TLen>(cap)
        {
        }

        usize write(byte) override
        {
            Std::panic("can't write raw bytes to a string");
        }

        usize write(Bytes) override
        {
            Std::panic("can't write raw bytes to a string");
        }

        usize writeStr(StringView str) override
        {
            _StringBuilder<E, TLen>::append(str);
            return str.len();
        }

        usize writeRune(Rune rune) override
        {
            _StringBuilder<E, TLen>::append(rune);
            return E::getRuneLength(rune);
        }

        usize writeUnit(Slice<typename E::Unit> unit)
        {
            _StringBuilder<E, TLen>::append(unit);
            return unit.len();
        }
    };

    template <usize TLen = 0>
    using StringWriter = _StringWriter<Encoding::UTF8, TLen>;
}