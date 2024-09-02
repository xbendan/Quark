#include <mixins/math/compute.h>
#include <mixins/std/c++types.h>

class Bitmap
{
public:
    Bitmap(usize size)
        : _data(new u64[divCeil(size, 64)])
        , _size(size)
        , _taken(0)
    {
    }
    Bitmap(Bitmap&& other)
        : _data(other._data)
        , _size(other._size)
        , _taken(other._taken)
    {
        other._data  = nullptr;
        other._size  = 0;
        other._taken = 0;
    }
    ~Bitmap() { delete[] _data; }

    void set(u64 position, usize amount = 1)
    {
        while (amount) {
            u8 bit = position % 64;
            if (bit + amount <= 64) {
                _data[position / 64] |= ((1ULL << amount) - 1) << bit;
                amount = 0;
            } else {
                usize taken = 64 - bit;
                _data[position / 64] |= ((1ULL << taken) - 1) << bit;
                position += taken;
                amount -= taken;
            }
        }
    }

    void clear(u64 position, usize amount = 1)
    {
        while (amount) {
            u8 bit = position % 64;
            if (bit + amount <= 64) {
                _data[position / 64] &= ~(((1ULL << amount) - 1) << bit);
                amount = 0;
            } else {
                usize taken = 64 - bit;
                _data[position / 64] &= ~(((1ULL << taken) - 1) << bit);
                position += taken;
                amount -= taken;
            }
        }
    }

    constexpr bool at(u64 position) const
    {
        return (position < _size) &&
               (_data[position / 64] & (1 << (position % 64)));
    }

    constexpr bool operator[](u64 position) const { return at(position); }

    u64 find(u64 amount)
    {
        u64 p = 0, free = 0;
        while (free < amount && p < _size) {
            if (~_data[p / 64] == 0) {
                free = 0;
                p += 64;
                continue;
            }

            if (!_data[p / 64]) {
                free += 64;
                p += 64;
            } else {
                free += (_data[p / 64] & (1 << (p % 64))) ? -free : 1;
                p++;
            }

            if (free >= amount) {
                return p - free;
            }
        }
        return 0;
    }

private:
    u64*  _data;
    usize _size;
    usize _taken;
};