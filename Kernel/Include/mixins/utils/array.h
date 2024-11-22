#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/slice.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/utility.h>
#include <mixins/utils/collections.h>

#include <mixins/std/initializer_list.h>

namespace Qk {
    template <typename, usize = 0>
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
            assert(index < TLen, "Index out of bounds");
            return _data[index];
        }

        always_inline constexpr TSource const& operator[](usize index) const
        {
            assert(index < TLen, "Index out of bounds");
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
    class Array<T, 0>
    {
    public:
        using InnerType = T;

        Array() = default;
        Array(usize size)
            : _data(new T[size])
            , _size(size)
        {
        }
        Array(T* data, usize size)
            : _data(data)
            , _size(size)
        {
        }
        Array(Array const& other)
            : _data(new T[other._size])
            , _size(other._size)
        {
            for (usize i = 0; i < _size; i++) {
                _data[i] = other._data[i];
            }
        }
        Array(Array&& other)
            : _data(other._data)
            , _size(other._size)
        {
            other._data = nullptr;
            other._size = 0;
        }
        Array(std::initializer_list<T> list)
            : _data(new T[list.size()])
            , _size(list.size())
        {
            usize i = 0;
            for (auto& item : list) {
                _data[i++] = Std::move(item);
            }
        }

        ~Array() { delete[] _data; }

        T& operator[](usize index) { return _data[index]; }

        T const& operator[](usize index) const { return _data[index]; }

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

        T const* buf() const { return _data; }

        usize len() const { return _size; }

        constexpr T* begin() { return _data; }

        constexpr T* end() { return _data + _size; }

    private:
        T*    _data = nullptr;
        usize _size = 0;
    };
}