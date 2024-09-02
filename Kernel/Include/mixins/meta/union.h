#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/std/math.h>
#include <mixins/std/new.h>
#include <mixins/std/utility.h>

template <typename... Ts>
class Union
{
public:
    [[gnu::always_inline]]
    Union() = delete;

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    Union(const _T& value)
        : _index(indexOf<_T, Ts...>())
    {
        new (_data) _T(value);
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    Union(_T&& value)
        : _index(indexOf<_T, Ts...>())
    {
        new (_data) _T(Std::move(value));
    }

    [[gnu::always_inline]]
    Union(const Union& other)
        : _index(other._index)
    {
        indexCast<Ts...>(
            _index, other._data, [this]<typename T>(const T& value) {
                new (_data) T(value);
            });
    }

    [[gnu::always_inline]]
    Union(Union&& other)
        : _index(other._index)
    {
        indexCast<Ts...>(_index, _data, [this]<typename T>(T& value) {
            new (_data) T(Std::move(value));
        });
    }

    [[gnu::always_inline]]
    ~Union()
    {
        indexCast<Ts...>(
            _index, _data, []<typename T>(T& value) { value.~T(); });
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    Union& operator=(const _T& value)
    {
        indexCast<Ts...>(_index, _data, [this]<typename U>(U& value) {
            value.T();
            new (_data) _T{ value };
        });
        return *this;
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    Union& operator=(_T&& value)
    {
        indexCast<Ts...>(_index, _data, [this]<typename U>(U& value) {
            value.T();
            new (_data) _T{ value };
        });
        _index = indexOf<_T, Ts...>();
        new (_data) _T{ Std::move(value) };

        return *this;
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    _T& unwrap()
    {
        if (_index != indexOf<_T, Ts...>()) [[unlikely]] {
            // panic
        }

        return *reinterpret_cast<_T*>(_data);
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    const _T& unwrap() const
    {
        if (_index != indexOf<_T, Ts...>()) [[unlikely]] {
            // panic
        }

        return *reinterpret_cast<const _T*>(_data);
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    const _T* is() const
    {
        return _index == indexOf<_T, Ts...>() ? (_T*)_data : nullptr;
    }

    template <Any<Ts...> _T>
    [[gnu::always_inline]]
    _T* is()
    {
        return _index == indexOf<_T, Ts...>() ? (_T*)_data : nullptr;
    }

    [[gnu::always_inline]]
    usize index() const
    {
        return _index;
    }

private:
    alignas(max(alignof(Ts)...)) char _data[max(sizeof(Ts)...)];
    u8 _index;
};