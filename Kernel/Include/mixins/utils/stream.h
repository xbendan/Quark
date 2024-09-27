#include <mixins/meta/func.h>
#include <mixins/meta/opt.h>
#include <mixins/std/type_traits.h>
#include <mixins/utils/collections.h>

template <typename TSource>
class Stream;

using IntStream    = Stream<int>;
using DoubleStream = Stream<double>;
using LongStream   = Stream<long>;

template <typename TSource>
class Stream
{
public:
    Stream(usize size)
        : _data(new TSource[size])
        , _size(size)
        , _p(0)
    {
    }
    Stream(Stream&& other)
        : _data(other._data)
        , _size(other._size)
        , _p(other._p)
    {
        other._data = nullptr;
        other._size = 0;
        other._p    = 0;
    }
    ~Stream() { delete[] _data; }

    // Intermediate operations

    Stream& filter(Predicate<TSource> pred)
    {
        usize p = 0;
        for (usize i = _p; i < _size; ++i) {
            if (pred(_data[i]) && i != p)
                _data[p++] = _data[i];
        }
        _p    = 0;
        _size = p;
        return *this;
    }

    template <typename R>
    Stream<R>& map(Func<R(TSource)> func)
    {
        Stream<R> stream(_size - _p);
        for (usize i = _p; i < _size; ++i) {
            stream._data[i - _p] = func(_data[i]);
        }
        return stream;
    }

    IntStream& mapToInt(Func<int(TSource)> func) { return map(func); }

    DoubleStream& mapToDouble(Func<double(TSource)> func) { return map(func); }

    DoubleStream& mapToDouble(Func<double(TSource)> func)
        requires(sizeof(TSource) == sizeof(double))
    {
        double* data = reinterpret_cast<double*>(_data);
        for (usize i = _p; i < _size; ++i) {
            data[i] = func(data[i]);
        }
        return *this;
    }

    LongStream& mapToLong(Func<long(TSource)> func) { return map(func); }

    LongStream& mapToLong(Func<long(TSource)> func)
        requires(sizeof(TSource) == sizeof(long))
    {
        long* data = reinterpret_cast<long*>(_data);
        for (usize i = _p; i < _size; ++i) {
            data[i] = func(data[i]);
        }
        return *this;
    }

    Stream& limit(usize size) { _size -= size; }

    Stream& skip(usize n)
    {
        _p += n;
        return *this;
    }

    Stream& distinct()
    {
        usize p = 0;
        for (usize i = _p; i < _size; ++i) {
            bool found = false;
            for (usize j = _p; j < i; ++j) {
                if (_data[i] == _data[j]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                _data[p++] = _data[i];
            }
        }
        return *this;
    }

    Stream& sorted()
        requires Comparable<TSource>
    {
        return sorted([](TSource a, TSource b) { return a < b; });
    }

    // TODO: Implement a more efficient sorting algorithm
    Stream& sorted(Comparator<TSource> comp)
    {
        for (usize i = _p; i < _size; ++i) {
            for (usize j = i + 1; j < _size; ++j) {
                if (comp(_data[i], _data[j])) {
                    TSource tmp = _data[i];
                    _data[i]    = _data[j];
                    _data[j]    = tmp;
                }
            }
        }
        return *this;
    }

    Stream& peek(Func<void(TSource)> func)
    {
        for (usize i = _p; i < _size; ++i) {
            func(_data[i]);
        }
        return *this;
    }

    Stream& truncate()
    {
        TSource* newData = new TSource[_size - _p];
        for (usize i = _p; i < _size; ++i) {
            newData[i - _p] = _data[i];
        }
        delete[] _data;
        _data = newData;
        return *this;
    }

    Optional<TSource> reduce(BinaryOperator<TSource> op)
    {
        if (_size == 0) {
            return Optional<TSource>();
        }
        TSource result = _data[_p];
        for (usize i = _p + 1; i < _size; ++i) {
            result = op(result, _data[i]);
        }
        return Optional<TSource>(result);
    }

    TSource reduce(TSource identity, BinaryOperator<TSource> op)
    {
        TSource result = identity;
        for (usize i = _p; i < _size; ++i) {
            result = op(result, _data[i]);
        }
        return result;
    }

    // Terminal operations

    void forEach(Func<void(TSource)> func)
    {
        for (usize i = _p; i < _size; ++i) {
            func(_data[i]);
        }
    }

    bool anyMatch(Predicate<TSource> pred)
    {
        for (usize i = _p; i < _size; ++i) {
            if (pred(_data[i])) {
                return true;
            }
        }
        return false;
    }

    bool allMatch(Predicate<TSource> pred)
    {
        for (usize i = _p; i < _size; ++i) {
            if (!pred(_data[i])) {
                return false;
            }
        }
        return true;
    }

    bool noneMatch(Predicate<TSource> pred)
    {
        for (usize i = _p; i < _size; ++i) {
            if (pred(_data[i])) {
                return false;
            }
        }
        return true;
    }

    Optional<TSource> findFirst()
    {
        if (_size == 0) {
            return Optional<TSource>();
        }
        return Optional<TSource>(_data[_p]);
    }

    Optional<TSource> findAny() {}

    usize count() { return _size - _p; }

    template <Collection<TSource> TKey>
    TKey collect(TKey& container)
    {
        for (usize i = _p; i < _size; ++i) {
            container.add(_data[i]);
        }
        return container;
    }

private:
    TSource* _data;
    usize    _size;
    usize    _p;
};