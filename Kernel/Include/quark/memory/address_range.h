#pragma once

#include <mixins/math/align.h>
#include <mixins/math/range.h>
#include <mixins/meta/opt.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>

namespace Quark::System::Memory {
    enum class MemoryType
    {
        USABLE,
        RESERVED,
        RECLAIMABLE,
        KERNEL
    };

    struct AddressRange
    {
        u64 _base, _size;

        template <typename T, typename U>
            requires((Std::isIntegral<T> || Std::isPointer<T>) and
                     (Std::isIntegral<U>))
        constexpr AddressRange(T base, U size)
            : _base((u64)base)
            , _size((u64)size)
        {
        }

        constexpr AddressRange(u64 base, u64 size)
            : _base(base)
            , _size(size)
        {
        }

        constexpr AddressRange(Range<u64> range)
            : _base(range._min)
            , _size(range._max - range._min)
        {
        }

        constexpr AddressRange() = default;

        constexpr AddressRange& operator=(const AddressRange& other)
        {
            _base = other._base;
            _size = other._size;
            return *this;
        }

        template <typename V>
        constexpr AddressRange& operator=(Range<V> range) noexcept
        {
            _base = (u64)range._min;
            _size = (u64)range._max - range._min;
            return *this;
        }

        constexpr bool operator==(const AddressRange& other) const
        {
            return _base == other._base && _size == other._size;
        }

        AddressRange& Clear()
        {
            Set(0);
            return *this;
        }

        AddressRange& Set(u8 val)
        {
            if (_size)
                for (u64 i = 0; i < _size; i++) {
                    ((u8*)_base)[i] = val;
                }

            return *this;
        }

        Optional<u64> Find(u8* val, u64 size, u64 offset = 0x1)
        {
            if (!_size || !_base) {
                return Empty();
            }
            for (u64 i = 0; i < _size; i += offset) {
                if (((u8*)_base)[i] == val[0]) {
                    u64 j = 0;
                    for (; j < size; j++) {
                        if (((u8*)_base)[i + j] != val[j]) {
                            break;
                        }
                    }
                    if (j == size) {
                        return i;
                    }
                }
            }
            return Empty();
        }

        template <typename TNumType>
        Optional<u64> Find(TNumType* val, u64 size, u64 offset = 0x1)
        {
            return Find((u8*)val, size * sizeof(TNumType), offset);
        }

        Optional<u64> TakeFront(u64 size)
        {
            if (_size < size) {
                return Empty();
            }
            u64 base = _base;
            _base += size;
            _size -= size;
            return base;
        }

        Optional<u64> TakeBack(u64 size)
        {
            if (_size < size) {
                return Empty();
            }
            _size -= size;
            return _base + _size;
        }

        AddressRange& InnerAlign(u64 alignment)
        {
            u64 limit = _base + _size;
            _base     = alignUp(_base, alignment);
            _size     = alignDown(limit, alignment) - _base;

            return *this;
        }
    };
} // namespace Quark::System::Memory