#include <mixins/std/c++types.h>
#include <mixins/std/typeinfo.h>
#include <mixins/str/string.h>

template <typename T>
struct Hash
{
public:
    Hash(String name)
        : _salt(OfValue((u8*)name.buf(), name.len()))
    {
    }

    Hash()
        : Hash(Type::GetName<T>())
    {
    }

    u64 operator()(T const& value) const { return OfType(value) * _salt; }

    static inline u64 OfValue(u8* data, usize len)
    {
        u64 hash = 0;
        for (usize i = 0; i < len; i++) {
            hash = data[i] + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    template <typename THashType>
    static inline u64 OfType(THashType& type)
    {
        return OfValue((u8*)&type, sizeof(THashType));
    }

    template <typename THashType>
    static inline u64 OfSlice(Slice<THashType> slice)
    {
        return OfValue((u8*)slice.buf(), slice.len() * sizeof(THashType));
    }

private:
    u64 _salt{ 0 };
};