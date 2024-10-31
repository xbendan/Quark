#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/slice.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/utility.h>
#include <mixins/utils/collections.h>

template <typename, usize>
class Array;

template <typename TSource, usize TLen>
class Array
{
public:
    using InnerType = TSource;

    always_inline constexpr Array() {}

    always_inline constexpr Array(TSource const (&data)[TLen])
        : _data{ data }
    {
    }

    always_inline constexpr Array(Array const& other)
        : _data{ other._data }
    {
    }

    always_inline constexpr Array(Array&& other)
        : _data{ Std::move(other._data) }
    {
    }

    always_inline constexpr TSource& operator[](usize index)
    {
        MakeAssertion(index < TLen, "Index out of bounds");
        return _data[index];
    }

    always_inline constexpr TSource const& operator[](usize index) const
    {
        MakeAssertion(index < TLen, "Index out of bounds");
        return _data[index];
    }

    TSource const* buf() const { return _data; }

    usize len() const { return TLen; }

    always_inline constexpr TSource* begin() { return _data; }

    always_inline constexpr TSource* end() { return _data + TLen; }

private:
    TSource _data[TLen];
};

static_assert(Sliceable<Array<int, 16>>);

template <typename T>
class Array<T, 0> : Slice<T>
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

    T const* buf() const { return _data; }

    usize len() const { return _size; }

    constexpr T* begin() { return _data; }

    constexpr T* end() { return _data + _size; }

private:
    T*    _data = nullptr;
    usize _size = 0;
};