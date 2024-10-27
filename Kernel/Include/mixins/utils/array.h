#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/slice.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/utility.h>
#include <mixins/utils/collections.h>

template <typename>
class Array;

template <typename T, usize N>
class Array<T[N]> : public Slice<T>
{
public:
    using InnerType = T;

    Array()
        : Slice<T>(_data, N)
    {
    }
    Array(Array const& other)
        : Slice<T>(_data, N)
    {
        for (usize i = 0; i < N; i++) {
            _data[i] = other._data[i];
        }
    }
    Array(Array&& other)
        : Slice<T>(_data, N)
    {
        for (usize i = 0; i < N; i++) {
            _data[i] = Std::move(other._data[i]);
        }
    }
    ~Array() = default;

    T& operator[](usize index) { return _data[index]; }

    T const& operator[](usize index) const { return _data[index]; }

    T const* Data() const { return &_data; }

    usize Length() const { return N; }

    constexpr T* begin() { return _data; }

    constexpr T* end() { return _data + N; }

private:
    T _data[N];
};

static_assert(Sliceable<Array<int[16]>>);

template <typename T>
class Array : Slice<T>
{
public:
    Array() = default;
    Array(usize size)
        : _data(new T[size])
        , _size(size)
    {
        this->_buf = _data;
        this->_len = size;
    }
    Array(T* data, usize size)
        : Slice<T>{ data, size }
        , _data(data)
        , _size(size)
    {
    }
    Array(const Array& other)
        : _data(new T[other._size])
        , _size(other._size)
    {
        this->_buf = _data;
        this->_len = _size;
        for (usize i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }
    }
    Array(Array&& other)
        : Slice<T>{ other._data, other._size }
        , _data(other._data)
        , _size(other._size)
    {
        other._data = nullptr;
        other._size = 0;
    }
    ~Array() { delete[] _data; }

    T& operator[](usize index) { return _data[index]; }

    void operator=(const Array& other)
    {
        delete[] _data;
        _data = new T[other._size];
        _size = other._size;
        for (usize i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }
    }

    void operator=(Array&& other)
    {
        delete[] _data;

        _data       = other._data;
        _size       = other._size;
        other._data = nullptr;
        other._size = 0;
    }

    T const& operator[](usize index) const { return _data[index]; }

    T const* Data() const { return _data; }

    usize Length() const { return _size; }

    constexpr T* begin() { return _data; }

    constexpr T* end() { return _data + _size; }

private:
    T*    _data = nullptr;
    usize _size = 0;
};

template <typename TSource>
class ArrayIterator : public IIterator<TSource>
{
public:
    ArrayIterator(TSource* data, usize size)
        : _data(data)
        , _size(size)
        , _index(0)
    {
    }

    TSource& current() const override { return _data[_index]; }

    TSource& next() override
    {
        MakeAssertion(_index < _size,
                      Error::IndexOutOfBounds("Index out of bounds"));
        return _data[_index++];
    }

    bool hasNext() const override { return _index < _size; }

private:
    TSource* _data;
    usize    _size;
    usize    _index;
};

class Arrays
{
    template <typename TSource>
    static void copyArray(TSource* dest, TSource const* src, usize count)
    {
        for (usize i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }

    template <typename TSource>
    static void copyArray(Slice<TSource>&       dest,
                          Slice<TSource> const& src,
                          usize                 count)
    {
        for (usize i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }

    template <typename TSource>
        requires(Computable<TSource>)
    static TSource sum(Slice<TSource> const& src)
    {
        TSource result = 0;
        for (usize i = 0; i < src.Length(); i++) {
            result += src[i];
        }
        return result;
    }

    template <typename TSource>
        requires(Computable<TSource>)
    static TSource average(Slice<TSource> const& src)
    {
        return sum(src) / src.Length();
    }

    template <typename TSource>
        requires(Comparable<TSource>)
    static TSource max(Slice<TSource> const& src)
    {
        TSource result = src[0];
        for (usize i = 1; i < src.Length(); i++) {
            if (src[i] > result) {
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource>
        requires(Comparable<TSource>)
    static TSource min(Slice<TSource> const& src)
    {
        TSource result = src[0];
        for (usize i = 1; i < src.Length(); i++) {
            if (src[i] < result) {
                result = src[i];
            }
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Computable<TResult>)
    static TResult sum(Slice<TSource> const&         src,
                       Func<TResult(TSource const&)> mapper)
    {
        TResult result = 0;
        for (usize i = 0; i < src.Length(); i++) {
            result += mapper(src[i]);
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Computable<TResult>)
    static TResult average(Slice<TSource> const&         src,
                           Func<TResult(TSource const&)> mapper)
    {
        return sum(src, mapper) / src.Length();
    }

    template <typename TSource, typename TResult>
        requires(Comparable<TResult>)
    static TResult max(Slice<TSource> const&         src,
                       Func<TResult(TSource const&)> mapper)
    {
        TResult result = mapper(src[0]);
        for (usize i = 1; i < src.Length(); i++) {
            if (mapper(src[i]) > result) {
                result = mapper(src[i]);
            }
        }
        return result;
    }

    template <typename TSource, typename TResult>
        requires(Comparable<TResult>)
    static TResult min(Slice<TSource> const&         src,
                       Func<TResult(TSource const&)> mapper)
    {
        TResult result = mapper(src[0]);
        for (usize i = 1; i < src.Length(); i++) {
            if (mapper(src[i]) < result) {
                result = mapper(src[i]);
            }
        }
        return result;
    }

    template <typename TSource, typename TKey>
        requires(Comparable<TKey>)
    static TSource maxBy(Slice<TSource> const&      src,
                         Func<TKey(TSource const&)> selector)
    {
        TSource result = src[0];
        TKey    key    = selector(src[0]);
        for (usize i = 1; i < src.Length(); i++) {
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
    static TSource minBy(Slice<TSource> const&      src,
                         Func<TKey(TSource const&)> selector)
    {
        TSource result = src[0];
        TKey    key    = selector(src[0]);
        for (usize i = 1; i < src.Length(); i++) {
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
    static Slice<TSource> order(Slice<TSource>& src, bool ascending = true)
    {
    }

    template <typename TSource, typename TKey>
    static Slice<TSource> orderBy(Slice<TSource>&            src,
                                  Func<TKey(TSource const&)> keySelector,
                                  bool                       ascending)
    {
    }

    template <typename TSource>
    static TSource findFirst(Slice<TSource>& src)
    {
        MakeAssertion(src.Length() > 0,
                      Error::IndexOutOfBounds("Array is empty"));
        return src[0];
    }

    template <typename TSource>
    static TSource findLast(Slice<TSource>& src)
    {
        MakeAssertion(src.Length() > 0,
                      Error::IndexOutOfBounds("Array is empty"));
        return src[src.Length() - 1];
    }

    template <typename TSource>
    static TSource findAny(Slice<TSource>& src)
    {
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> take(Slice<TSource>& src, usize n)
    {
        MakeAssertion(n <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));

        TSlice result{ new TSource[n], n };
        copyArray(result, src, n);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> take(Slice<TSource>& src, Tuple<usize, usize> range)
    {
        usize start = range.get<0>();
        usize end   = range.get<1>();

        MakeAssertion(start <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));
        MakeAssertion(end <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));

        usize           count = end - start;
        TSlice<TSource> result{ new TSource[count], count };
        copyArray(result.data(), &(src[start]), count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> takeLast(Slice<TSource>& src, usize n)
    {
        MakeAssertion(n <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));

        TSlice<TSource> result{ new TSource[n], n };
        copyArray(result.data(), &(src[src.Length() - n]), n);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> takeWhile(Slice<TSource>&     src,
                                     Predicate<TSource&> predicate)
    {
        usize count = 0;
        for (usize i = 0; i < src.Length(); i++) {
            if (predicate(src[i]))
                count++;
        }

        TSlice<TSource> result{ new TSource[count], count };
        usize           p = 0;
        for (usize i = 0; i < src.Length() && p != count; i++) {
            if (predicate(src[i]))
                result[p++] = src[i];
        }
        return result;
    }

    template <typename TSource>
    static TSource& single(Slice<TSource>& src)
    {
        MakeAssertion(src.Length() == 1,
                      Error::InvalidOperation("Array is not single"));
        return src[0];
    }

    template <typename TSource>
    static TSource& single(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        usize index = -1;
        for (usize i = 0; i < src.Length(); i++) {
            if (predicate(src[i])) {
                if (index != -1) {
                    MakeAssertion(
                        false,
                        Error::InvalidOperation(
                            "Sequence contains more than one element"));
                }
                index = i;
            }
        }
        MakeAssertion(index != -1,
                      Error::InvalidOperation("Sequence contains no element"));
        return src[index];
    }

    template <typename TSource>
    static TSource& singleOrDefault(Slice<TSource>& src,
                                    TSource const&  defaultValue)
    {
        MakeAssertion(
            src.Length() <= 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return src.Length() ? src[0] : defaultValue;
    }

    template <typename TSource>
        requires(Constructible<TSource>)
    static TSource& defaultIfEmpty(Slice<TSource>& src,
                                   TSource const&  defaultValue)
    {
        return src.Length() ? src[0] : defaultValue;
    }

    template <typename TSource>
    static TSource& defaultIfEmpty(Slice<TSource>& src)
    {
        return src.Length() ? src[0] : TSource{};
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> skip(Slice<TSource>& src, usize n)
    {
        MakeAssertion(n <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));

        usize           count = src.Length() - n;
        TSlice<TSource> result{ new TSource[count], count };
        copyArray(result.data(), &(src[n]), count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> skipLast(Slice<TSource>& src, usize n)
    {
        MakeAssertion(n <= src.Length(),
                      Error::IndexOutOfBounds("Array is too small"));

        usize           count = src.Length() - n;
        TSlice<TSource> result{ new TSource[count], count };
        copyArray(result, src, count);
        return result;
    }

    template <typename TSource, template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TSource>, TSource>)
    static TSlice<TSource> skipWhile(Slice<TSource>&     src,
                                     Predicate<TSource&> predicate)
    {
        usize count = 0;
        for (usize i = 0; i < src.Length(); i++) {
            if (!predicate(src[i]))
                count++;
        }

        TSlice<TSource> result{ new TSource[count], count };
        usize           p = 0;
        for (usize i = 0; i < src.Length() && p != count; i++) {
            if (!predicate(src[i]))
                result[p++] = src[i];
        }
        return result;
    }

    template <typename TSource,
              typename TResult,
              template <typename> typename TSlice = Array>
        requires(Sliceable<TSlice<TResult>, TSource>)
    static TSlice<TResult> select(Slice<TSource>&         src,
                                  Func<TResult(TSource&)> selector)
    {
        TSlice<TResult> result{ new TResult[src.Length()], src.Length() };
        for (usize i = 0; i < src.Length(); i++) {
            result[i] = selector(src[i]);
        }
        return result;
    }

    template <typename TSource>
    static bool allMatch(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.Length(); i++) {
            if (!predicate(src[i]))
                return false;
        }
        return true;
    }

    template <typename TSource>
    static bool anyMatch(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.Length(); i++) {
            if (predicate(src[i]))
                return true;
        }
        return false;
    }

    template <typename TSource>
    static bool noneMatch(Slice<TSource>& src, Predicate<TSource&> predicate)
    {
        for (usize i = 0; i < src.Length(); i++) {
            if (predicate(src[i]))
                return false;
        }
        return true;
    }

    template <typename TSource>
    static void forEach(Slice<TSource>& src, Action<TSource&> action)
    {
        for (usize i = 0; i < src.Length(); i++) {
            action(src[i]);
        }
    }
};