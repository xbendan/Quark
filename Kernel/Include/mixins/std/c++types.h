#pragma once

#define always_inline __attribute__((always_inline))

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using byte  = unsigned char;
using word  = unsigned short;
using dword = unsigned int;
using qword = unsigned long long;

using usize = long unsigned int;
using isize = long int;

using f32  = float;
using f64  = double;
using f128 = long double;

using nullptr_t = decltype(nullptr);

// using le8  = u8 __attribute__((bitwise));
// using le16 = u16 __attribute__((bitwise));
// using le32 = u32 __attribute__((bitwise));
// using le64 = u64 __attribute__((bitwise));

using le8  = u8;
using le16 = u16;
using le32 = u32;
using le64 = u64;

struct Empty
{
    constexpr Empty() = default;

    explicit constexpr operator bool() const { return false; }

    constexpr bool operator==(Empty const&) const = default;

    constexpr bool operator!=(Empty const&) const = default;
};

template <typename T>
bool
operator==(Empty, T* ptr)
{
    return ptr == nullptr;
}

struct Copy
{};

struct Move
{};