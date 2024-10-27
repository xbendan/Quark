#pragma once

#include <mixins/math/align.h>
#include <mixins/math/range.h>
#include <mixins/meta/opt.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>

namespace Quark::System::Memory {

    struct AddressRange : public Range<u64>
    {
        template <typename T, typename U>
            requires((Std::isIntegral<T> || Std::isPointer<T>) and
                     (Std::isIntegral<U>))
        constexpr AddressRange(T base, U size)
            : Range((u64)base, (u64)base + size)
        {
        }

        constexpr AddressRange(u64 base, u64 size)
            : Range(base, base + size)
        {
        }

        constexpr AddressRange(Range<u64> range)
            : Range(Std::move(range))
        {
        }

        constexpr AddressRange(const AddressRange& other)
            : Range(other)
        {
        }

        constexpr AddressRange(AddressRange&& other)
            : Range(Std::forward<Range>(other))
        {
        }

        constexpr AddressRange() = default;

        constexpr AddressRange& operator=(const AddressRange& other)
        {
            _min = other._min;
            _max = other._max;
            return *this;
        }

        template <typename V>
        constexpr AddressRange& operator=(Range<V> range) noexcept
        {
            _min = (u64)range._min;
            _max = (u64)range._max;
            return *this;
        }

        constexpr bool operator==(const AddressRange& other) const
        {
            return _min == other._min && _max == other._max;
        }

        inline u64 From() const { return _min; }

        inline u64 To() const { return _max; }

        inline u64 Length() const { return _max >= _min ? _max - _min : 0; }

        AddressRange& SetZero()
        {
            SetValue(0);
            return *this;
        }

        AddressRange& SetValue(u8 val)
        {
            u64 len = Length();
#if defined(__x86_64__)
            asm volatile("rep stosb"
                         : "+D"(_min), "+c"(len)
                         : "a"(val)
                         : "memory");
#elif
            for (u64 i = 0; i < len; i++) {
                ((u8*)_min)[i] = val;
            }
#endif
            return *this;
        }

        Optional<u64> Find(u8* val, u64 size, u64 offset = 0x1)
        {
            if (!_min || !_max) {
                return Empty();
            }

            for (u64 i = 0; i < Length(); i += offset) {
                if (((u8*)_min)[i] == val[0]) {
                    u64 j = 0;
                    for (; j < size; j++) {
                        if (((u8*)_min)[i + j] != val[j]) {
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
            if (Length() < size) {
                return Empty();
            }
            u64 address = _min;
            _min += size;

            return address;
        }

        Optional<u64> TakeBack(u64 size)
        {
            if (Length() < size) {
                return Empty();
            }
            _max -= size;

            return _max;
        }

        AddressRange& InnerAlign(u64 alignment)
        {
            Math::AlignUpRef(_min, alignment);
            Math::AlignDownRef(_max, alignment);

            return *this;
        }

        AddressRange& OuterAlign(u64 alignment)
        {
            Math::AlignDownRef(_min, alignment);
            Math::AlignUpRef(_max, alignment);

            return *this;
        }

        AddressRange& ConstraintsTo(AddressRange& other)
        {
            _min = ((Range<u64>)other).ConstraintsTo(_min);
            _max = ((Range<u64>)other).ConstraintsTo(_max);

            return *this;
        }

        void CopyTo(u8* dest)
        {
            u64 len = Length();
            for (u64 i = 0; i < len; i++) {
                dest[i] = ((u8*)_min)[i];
            }
        }
    };
} // namespace Quark::System::Memory