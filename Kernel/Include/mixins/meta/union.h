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
    always_inline Union() = delete;

    template <Any<Ts...> TAnyType>
    always_inline Union(const TAnyType& value)
        : _index(indexOf<TAnyType, Ts...>())
    {
        new (_data) TAnyType(value);
    }

    template <Any<Ts...> TAnyType>
    always_inline Union(TAnyType&& value)
        : _index(indexOf<TAnyType, Ts...>())
    {
        new (_data) TAnyType(Std::move(value));
    }

    always_inline Union(Union<Ts...> const& other)
        : _index(other._index)
    {
        indexCast<Ts...>(
            _index, other._data, [this]<typename T>(const T& value) {
                new (_data) T(value);
            });
    }

    always_inline Union(Union&& other)
        : _index(other._index)
    {
        indexCast<Ts...>(_index, _data, [this]<typename T>(T& value) {
            new (_data) T(Std::move(value));
        });
    }

    always_inline ~Union()
    {
        indexCast<Ts...>(
            _index, _data, []<typename T>(T& value) { value.~T(); });
    }

    template <Any<Ts...> TAnyType>
    always_inline Union& operator=(const TAnyType& value)
    {
        indexCast<Ts...>(_index, _data, [this]<typename U>(U& value) {
            value.T();
            new (_data) TAnyType{ value };
        });
        return *this;
    }

    template <Any<Ts...> TAnyType>
    always_inline Union& operator=(TAnyType&& value)
    {
        indexCast<Ts...>(_index, _data, [this]<typename U>(U& value) {
            value.T();
            new (_data) TAnyType{ value };
        });
        _index = indexOf<TAnyType, Ts...>();
        new (_data) TAnyType{ Std::move(value) };

        return *this;
    }

    always_inline Union& operator=(Union const& other)
    {
        indexCast<Ts...>(
            _index, _data, []<typename T>(T& value) { value.~T(); });
        _index = other._index;

        indexCast<Ts...>(_index, other._data, [this]<typename T>(T& value) {
            new (_data) T(value);
        });

        return *this;
    }

    always_inline Union& operator=(Union&& other)
    {
        indexCast<Ts...>(_index, _data, []<typename T>(T& ptr) { ptr.~T(); });
        _index = other._index;

        indexCast<Ts...>(_index, other._data, [this]<typename T>(T& ptr) {
            new (_data) T(Std::move(ptr));
        });

        return *this;
    }

    template <Any<Ts...> TAnyType>
    always_inline TAnyType& unwrap()
    {
        if (_index != indexOf<TAnyType, Ts...>()) [[unlikely]] {
            // panic
        }

        return *reinterpret_cast<TAnyType*>(_data);
    }

    template <Any<Ts...> TAnyType>
    always_inline const TAnyType& unwrap() const
    {
        if (_index != indexOf<TAnyType, Ts...>()) [[unlikely]] {
            // panic
        }

        return *reinterpret_cast<const TAnyType*>(_data);
    }

    template <Any<Ts...> TAnyType>
    always_inline const TAnyType* is() const
    {
        return _index == indexOf<TAnyType, Ts...>() ? (TAnyType*)_data
                                                    : nullptr;
    }

    template <Any<Ts...> TAnyType>
    always_inline TAnyType* is()
    {
        return _index == indexOf<TAnyType, Ts...>() ? (TAnyType*)_data
                                                    : nullptr;
    }

    always_inline usize index() const { return _index; }

private:
    alignas(max(alignof(Ts)...)) char _data[max(sizeof(Ts)...)];
    u8 _index;
};