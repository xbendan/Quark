#pragma once

#include <mixins/std/concepts.h>

template <typename T>
class Iterator
{
public:
    Iterator(usize size)
        : _data(new T[size])
        , _size(size)
        , _p(0)
    {
    }
    Iterator(T* data, usize size)
        : _data(data)
        , _size(size)
        , _p(0)
    {
    }
    Iterator(Iterator const& other)
        : _data(new T[other._size])
        , _size(other._size)
        , _p(other._p)
    {
        for (usize i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }
    }
    Iterator(Iterator&& other)
    {
        _data       = other._data;
        _size       = other._size;
        _p          = other._p;
        other._data = nullptr;
        other._size = 0;
        other._p    = 0;
    }
    ~Iterator() { delete[] _data; }

    bool hasNext() { return _p < _size; }

    T& next() { return _data[_p++]; }

private:
    T*    _data;
    usize _size;
    usize _p;
};

// template <typename T, typename E>
// concept Iterator = requires(T& t) {
//     { t.hasNext() } -> ConvertibleTo<bool>;
//     { t.next() } -> SameAs<E&>;
//     { t.remove() } -> SameAs<void>;
// };

template <typename T, typename E>
concept Iterable = requires(T t) {
    { t.iterator() } -> SameAs<Iterator<E>>;
};