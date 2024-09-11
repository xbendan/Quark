#include <mixins/meta/inert.h>
#include <mixins/meta/slice.h>
#include <mixins/std/math.h>
#include <mixins/utils/array.h>

template <typename T>
struct Buf
{
    using InnerType = T;

    Inert<T>* _data{};
    usize     _cap{};
    usize     _len{};

    Buf(usize capacity = 0) { reserve(capacity); }

    Buf(T* data, usize capacity, usize len = 0)
        : _data(data)
        , _cap(capacity)
        , _len(len)
    {
        for (usize i = 0; i < len; i++) {
            _data[i].ctor(data[i]);
        }
    }

    Buf(Sliceable<T> auto& other)
    {
        reserve(other.len());
        for (usize i = 0; i < other.len(); i++) {
            _data[i].ctor(other[i]);
        }
        _len = other.len();
    }
    Buf(Buf const& other)
    {
        reserve(other._len);

        _len = other._len;
        for (usize i = 0; i < _len; i++) {
            _data[i].ctor(other[i]);
        }
    }
    Buf(Buf&& other)
        : _data(other._data)
        , _cap(other._cap)
        , _len(other._len)
    {
        other._data = nullptr;
        other._cap  = 0;
        other._len  = 0;
    }

    ~Buf()
    {
        if (!_data)
            return;

        for (usize i = 0; i < _len; i++) {
            _data[i].dtor();
        }
        delete[] _data;
    }

    Buf& operator=(Buf const& other)
    {
        *this = Buf(other);
        return *this;
    }

    Buf& operator=(Buf&& other)
    {
        if (this == &other)
            return *this;

        if (_data) {
            for (usize i = 0; i < _len; i++) {
                _data[i].dtor();
            }
            delete[] _data;
        }

        _data = other._data;
        _cap  = other._cap;
        _len  = other._len;

        other._data = nullptr;
        other._cap  = 0;
        other._len  = 0;

        return *this;
    }

    constexpr T& operator[](usize i) { return _data[i].unwrap(); }

    constexpr T const& operator[](usize i) const { return _data[i].unwrap(); }

    void reserve(usize capacity)
    {
        if (capacity <= _cap) {
            return;
        }

        if (!_data) {
            _data = new Inert<T>[capacity];
            _cap  = capacity;
            return;
        }

        usize n = max(capacity, _cap * 2);

        Inert<T>* data = new Inert<T>[n];
        for (usize i = 0; i < _len; i++) {
            data[i].ctor(_data[i].take());
            _data[i].dtor();
        }

        delete[] _data;
        _data = data;
        _cap  = n;
    }

    void resize(usize len, T fill = {})
    {
        if (len == _len || !len)
            return;

        Inert<T>* data = new Inert<T>[len];
        for (usize i = 0; i < len; i++) {
            if (i < _len) {
                data[i].ctor(_data[i].take());
                _data[i].dtor();
            } else {
                data[i].ctor(fill);
            }
        }

        delete[] _data;
        _data = data;
        _len  = len;
        _cap  = len;
    }

    void truncate(usize len)
    {
        if (len >= _len)
            return;

        for (usize i = len; i < _len; i++) {
            _data[i].dtor();
        }

        _len = len;
    }

    T* take()
    {
        T* data = buf();
        _data   = nullptr;
        _len    = 0;
        _cap    = 0;

        return data;
    }

    T* buf()
    {
        if (!_data)
            return nullptr;
        return &(_data->unwrap());
    }

    T const* buf() const
    {
        if (!_data)
            return nullptr;
        return &(_data->unwrap());
    }

    usize len() const { return _len; }
};

template <typename T, usize N>
struct Buf<T[N]>
{
    using InnerType = T;

    Array<Inert<T>[N]> _data;
    usize              _len{};
};