#include <mixins/std/c++types.h>
#include <mixins/std/string.h>

template <typename T>
class Traits;

template <>
class Traits<u8>
{
public:
    using Type       = u8;
    using BufferType = char;

    static constexpr bool  isSigned   = false;
    static constexpr bool  isIntegral = true;
    static constexpr usize bufferSize = 3;

    static u64    hashCode(Type const& value);
    static string toString(Type const& value);
};

template <>
class Traits<u16>
{
public:
    using Type       = u16;
    using BufferType = char;

    static constexpr bool  isSigned   = false;
    static constexpr bool  isIntegral = true;
    static constexpr usize bufferSize = 5;

    static u64    hashCode(Type const& value);
    static string toString(Type const& value);
};

template <>
class Traits<u32>
{
public:
    using Type       = u32;
    using BufferType = char;

    static constexpr bool  isSigned   = false;
    static constexpr bool  isIntegral = true;
    static constexpr usize bufferSize = 10;

    static u64    hashCode(Type const& value);
    static string toString(Type const& value);
};

template <>
class Traits<u64>
{
public:
    using Type       = u64;
    using BufferType = char;

    static constexpr bool  isSigned   = false;
    static constexpr bool  isIntegral = true;
    static constexpr usize bufferSize = 20;

    static u64    hashCode(Type const& value);
    static string toString(Type const& value);
};
