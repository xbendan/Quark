#pragma once

#include <mixins/meta/buf.h>
#include <mixins/meta/iter.h>
#include <mixins/std/utility.h>
#include <mixins/str/string.h>

namespace Qk {
    template <EncodingFormat, usize = 0>
    struct _StringBuilder;

    template <EncodingFormat E, usize TLen>
    struct _StringBuilder
    {
        using Encoding = E;
        using Unit     = typename E::Unit;

        Buf<Unit, TLen> _buf;

        _StringBuilder() = default;

        void ensure(usize cap)
        {
            if (cap > TLen)
                Std::panic("StringBuilder: buffer overflow");
        }

        void append(Rune rune)
        {
            typename E::One one;
            if (not E::encodeUnit(rune, one))
                return;

            for (auto unit : iter(one))
                _buf.insert(_buf.len(), Std::move(unit));
        }

        void append(Sliceable<Rune> auto const& runes)
        {
            for (auto rune : runes)
                append(rune);
        }

        void append(Sliceable<typename E::Unit> auto const& units)
        {
            _buf.insert(Copy{}, _buf.len(), units.buf(), units.len());
        }

        usize len() const { return _buf.len(); }

        _StringView<E> getStringView() { return _buf; }

        void clear() { _buf.trunc(0); }

        _String<E> getString()
        {
            usize len = _buf.len();
            _buf.insert(len, 0);
            return { Copy{}, _buf.take(), len };
        }
    };

    template <EncodingFormat E>
    struct _StringBuilder<E, 0>
    {
        using Encoding = E;
        using Unit     = typename E::Unit;

        Buf<Unit> _buf;

        _StringBuilder(usize cap = 16)
            : _buf(cap)
        {
        }

        void ensure(usize cap)
        {
            // NOTE: This way client code don't have to take
            //       the null-terminator into account
            _buf.ensure(cap + 1);
        }

        void append(Rune rune)
        {
            typename E::One one;
            if (not E::encodeUnit(rune, one))
                return;

            for (auto unit : iter(one))
                _buf.insert(_buf.len(), Std::move(unit));
        }

        void append(Sliceable<Rune> auto const& runes)
        {
            for (auto rune : runes)
                append(rune);
        }

        void append(Sliceable<typename E::Unit> auto const& units)
        {
            _buf.insert(Copy{}, _buf.len(), units.buf(), units.len());
        }

        usize len() const { return _buf.size(); }

        _StringView<E> getStringView() { return _buf; }

        void clear() { _buf.trunc(0); }

        _String<E> getString()
        {
            usize len = _buf.len();
            _buf.insert(len, 0);
            return { Move{}, _buf.take(), len };
        }
    };

    template <usize TLen = 0>
    using StringBuilder = _StringBuilder<Encoding::UTF8, TLen>;
}