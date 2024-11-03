#include <mixins/meta/cursor.h>
#include <mixins/str/encode.h>
#include <mixins/str/string_view.h>

namespace Qk {
    template <EncodingFormat E>
    struct _Scan
    {
        using Encoding = E;
        using Unit     = typename E::Unit;

        Cursor<Unit> _cursor;
        Cursor<Unit> _begin;

        constexpr _Scan(_StringView<E> view)
            : _cursor(view)
        {
        }

        bool ended() const { return _cursor.ended(); }

        usize rem()
        {
            auto curr = _cursor;

            usize result = 0;
            while (curr.rem()) {
                u32 r;
                if (E::decodeUnit(r, curr)) {
                    result++;
                }
            }

            return result;
        }

        Rune peek()
        {
            if (ended())
                return '\0';

            Rune r;
            auto curr = _cursor;
            return E::decodeUnit(r, curr) ? r : INVALID_SYMBOL;
        }

        Rune peek(usize count)
        {
            auto save = _cursor;
            next(count);
            auto r  = peek();
            _cursor = save;
            return r;
        }

        Rune next()
        {
            if (ended())
                return '\0';
            Rune r;
            return E::decodeUnit(r, _cursor) ? r : INVALID_SYMBOL;
        }

        Rune next(usize count)
        {
            Rune r = '\0';
            for (usize i = 0; i < count; i++) {
                r = next();
            }
            return r;
        }

        auto& begin()
        {
            _begin = _cursor;
            return *this;
        }

        _StringView<E> end() { return { _begin, _cursor }; }

        _StringView<E> slice(usize n)
        {
            auto begin = _cursor;
            for (usize i = 0; i < n; i++) {
                next(n);
            }
            return { begin, _cursor };
        }

        bool skip(Rune c)
        {
            if (peek() == c) {
                next();
                return true;
            }
            return false;
        }

        bool skip(StringView str) { return false; }

        bool eat(Rune c)
        {
            bool result = false;
            if (skip(c)) {
                result = true;
                while (skip(c) and !ended())
                    ;
            }
            return result;
        }

        bool eat(StringView str)
        {
            bool result = false;
            if (str.isEmpty())
                return false;

            auto save = _cursor;
            for (usize i = 0; i < str.len(); i++) {
                if (str[i] != peek()) {
                    _cursor = save;
                    return false;
                }
                next();
            }
            return true;
        }
    };

    using Scan = _Scan<Encoding::UTF8>;
}