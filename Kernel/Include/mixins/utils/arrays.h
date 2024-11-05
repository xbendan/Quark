#include <mixins/meta/error.h>
#include <mixins/meta/func.h>
#include <mixins/meta/slice.h>
#include <mixins/utils/array.h>

namespace Qk::Arrays {
    template <typename TSource>
    void CopyArray(TSource* dest, TSource const* src, usize count)
    {
        for (usize i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }

    template <typename TSource>
    void CopyArray(Slice<TSource>& dest, Slice<TSource> const& src, usize count)
    {
        for (usize i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }

    template <typename TSource>
        requires(Computable<TSource>)
    TSource sum(Slice<TSource> const& src)
    {
        TSource result = 0;
        for (usize i = 0; i < src.len(); i++) {
            result += src[i];
        }
        return result;
    }

    template <typename TSource>
        requires(Computable<TSource>)
    TSource average(Slice<TSource> const& src)
    {
        return sum(src) / src.len();
    }

    template <typename TSource>
        requires(Comparable<TSource>)
    TSource max(Slice<TSource> const& src)
    {
        TSource result = src[0];
        for (usize i = 1; i < src.len(); i++) {
            if (src[i] > result) {
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource>
        requires(Comparable<TSource>)
    TSource min(Slice<TSource> const& src)
    {
        TSource result = src[0];
        for (usize i = 1; i < src.len(); i++) {
            if (src[i] < result) {
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Computable<TResult>)
    TResult sum(Slice<TSource> const& src, Func<TResult(TSource const&)> mapper)
    {
        TResult result = 0;
        for (usize i = 0; i < src.len(); i++) {
            result += mapper(src[i]);
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Computable<TResult>)
    TResult average(Slice<TSource> const&         src,
                    Func<TResult(TSource const&)> mapper)
    {
        return sum(src, mapper) / src.len();
    }

    template <typename TSource, typename TResult>
        requires(Comparable<TResult>)
    TResult max(Slice<TSource> const& src, Func<TResult(TSource const&)> mapper)
    {
        TResult result = mapper(src[0]);
        for (usize i = 1; i < src.len(); i++) {
            if (mapper(src[i]) > result) {
                result = mapper(src[i]);
            }
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Comparable<TResult>)
    TResult min(Slice<TSource> const& src, Func<TResult(TSource const&)> mapper)
    {
        TResult result = mapper(src[0]);
        for (usize i = 1; i < src.len(); i++) {
            if (mapper(src[i]) < result) {
                result = mapper(src[i]);
            }
        }
        return result;
    }

    template <typename TSource, typename TKey>
        requires(Comparable<TKey>)
    TSource maxBy(Slice<TSource> const&      src,
                  Func<TKey(TSource const&)> selector)
    {
        TSource result = src[0];
        TKey    key    = selector(src[0]);
        for (usize i = 1; i < src.len(); i++) {
            TKey current = selector(src[i]);
            if (current > key) {
                key    = current;
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource, typename TKey>
        requires(Comparable<TKey>)
    TSource minBy(Slice<TSource> const&      src,
                  Func<TKey(TSource const&)> selector)
    {
        TSource result = src[0];
        TKey    key    = selector(src[0]);
        for (usize i = 1; i < src.len(); i++) {
            TKey current = selector(src[i]);
            if (current < key) {
                key    = current;
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource>
        requires(Comparable<TSource>)
    Slice<TSource> order(Slice<TSource>& src, bool ascending = true)
    {
    }

    template <typename TSource, typename TKey>
    Slice<TSource> orderBy(Slice<TSource>&            src,
                           Func<TKey(TSource const&)> keySelector,
                           bool                       ascending)
    {
    }

    template <typename TSource>
    TSource findFirst(Slice<TSource>& src)
    {
        assert(src.len() > 0, Error::IndexOutOfBounds("Array is empty"));
        return src[0];
    }

    template <typename TSource>
    TSource findLast(Slice<TSource>& src)
    {
        assert(src.len() > 0, Error::IndexOutOfBounds("Array is empty"));
        return src[src.len() - 1];
    }

    template <typename TSource>
    TSource findAny(Slice<TSource>& src)
    {
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> take(Slice<TSource>& src, usize n)
    {
        assert(n <= src.len(), Error::IndexOutOfBounds("Array is too small"));

        TSlice result{ new TSource[n], n };
        CopyArray(result, src, n);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> take(Slice<TSource>& src, Tuple<usize, usize> range)
    {
        usize start = range.v0;
        usize end   = range.v1;

        assert(start <= src.len(),
               Error::IndexOutOfBounds("Array is too small"));
        assert(end <= src.len(), Error::IndexOutOfBounds("Array is too small"));

        usize           count = end - start;
        TSlice<TSource> result{ new TSource[count], count };
        CopyArray(result.buf(), &(src[start]), count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> takeLast(Slice<TSource>& src, usize n)
    {
        assert(n <= src.len(), Error::IndexOutOfBounds("Array is too small"));

        TSlice<TSource> result{ new TSource[n], n };
        CopyArray(result.buf(), &(src[src.len() - n]), n);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> takeWhile(Slice<TSource>&     src,
                              Predicate<TSource&> predicate)
    {
        usize count = 0;
        for (usize i = 0; i < src.len(); i++) {
            if (predicate(src[i]))
                count++;
        }

        TSlice<TSource> result{ new TSource[count], count };
        usize           p = 0;
        for (usize i = 0; i < src.len() && p != count; i++) {
            if (predicate(src[i]))
                result[p++] = src[i];
        }
        return result;
    }

    template <typename TSource>
    TSource& single(Slice<TSource>& src)
    {
        assert(src.len() == 1, Error::InvalidOperation("Array is not single"));
        return src[0];
    }

    template <typename TSource>
    TSource& single(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        usize index = -1;
        for (usize i = 0; i < src.len(); i++) {
            if (predicate(src[i])) {
                if (index != -1) {
                    assert(false,
                           Error::InvalidOperation(
                               "Sequence contains more than one element"));
                }
                index = i;
            }
        }
        assert(index != -1,
               Error::InvalidOperation("Sequence contains no element"));
        return src[index];
    }

    template <typename TSource>
    TSource& singleOrDefault(Slice<TSource>& src, TSource const& defaultValue)
    {
        assert(
            src.len() <= 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return src.len() ? src[0] : defaultValue;
    }

    template <typename TSource>
        requires(Constructible<TSource>)
    TSource& defaultIfEmpty(Slice<TSource>& src, TSource const& defaultValue)
    {
        return src.len() ? src[0] : defaultValue;
    }

    template <typename TSource>
    TSource& defaultIfEmpty(Slice<TSource>& src)
    {
        return src.len() ? src[0] : TSource{};
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> skip(Slice<TSource>& src, usize n)
    {
        assert(n <= src.len(), Error::IndexOutOfBounds("Array is too small"));

        usize           count = src.len() - n;
        TSlice<TSource> result{ new TSource[count], count };
        CopyArray(result.buf(), &(src[n]), count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> skipLast(Slice<TSource>& src, usize n)
    {
        assert(n <= src.len(), Error::IndexOutOfBounds("Array is too small"));

        usize           count = src.len() - n;
        TSlice<TSource> result{ new TSource[count], count };
        CopyArray(result, src, count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    TSlice<TSource> skipWhile(Slice<TSource>&     src,
                              Predicate<TSource&> predicate)
    {
        usize count = 0;
        for (usize i = 0; i < src.len(); i++) {
            if (!predicate(src[i]))
                count++;
        }

        TSlice<TSource> result{ new TSource[count], count };
        usize           p = 0;
        for (usize i = 0; i < src.len() && p != count; i++) {
            if (!predicate(src[i]))
                result[p++] = src[i];
        }
        return result;
    }

    template <typename TSource,
              typename TResult,
              template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TResult>, TSource>)
    TSlice<TResult> select(Slice<TSource>&         src,
                           Func<TResult(TSource&)> selector)
    {
        TSlice<TResult> result{ new TResult[src.len()], src.len() };
        for (usize i = 0; i < src.len(); i++) {
            result[i] = selector(src[i]);
        }
        return result;
    }

    template <typename TSource>
    bool allMatch(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.len(); i++) {
            if (!predicate(src[i]))
                return false;
        }
        return true;
    }

    template <typename TSource>
    bool AnyMatch(Sliceable<TSource> auto& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.len(); i++) {
            if (predicate(src[i]))
                return true;
        }
        return false;
    }

    template <typename TSource>
    bool noneMatch(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.len(); i++) {
            if (predicate(src[i]))
                return false;
        }
        return true;
    }

    template <typename TSource>
    void ForEach(Sliceable<TSource> auto& src, Action<TSource&> action)
    {
        for (usize i = 0; i < src.len(); i++) {
            action(src[i]);
        }
    }
}