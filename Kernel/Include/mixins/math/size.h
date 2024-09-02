
inline constexpr auto
KiB(auto n)
{
    return n * 1024;
}

inline constexpr auto
MiB(auto n)
{
    return KiB(n) * 1024;
}

inline constexpr auto
GiB(auto n)
{
    return MiB(n) * 1024;
}

inline constexpr auto
TiB(auto n)
{
    return GiB(n) * 1024;
}

inline constexpr auto
PiB(auto n)
{
    return TiB(n) * 1024;
}

inline constexpr auto
EiB(auto n)
{
    return PiB(n) * 1024;
}

inline constexpr auto
ZiB(auto n)
{
    return EiB(n) * 1024;
}

inline constexpr auto
YiB(auto n)
{
    return ZiB(n) * 1024;
}

inline constexpr auto
KB(auto n)
{
    return n * 1000;
}

inline constexpr auto
MB(auto n)
{
    return KB(n) * 1000;
}

inline constexpr auto
GB(auto n)
{
    return MB(n) * 1000;
}

inline constexpr auto
TB(auto n)
{
    return GB(n) * 1000;
}

inline constexpr auto
PB(auto n)
{
    return TB(n) * 1000;
}

inline constexpr auto
EB(auto n)
{
    return PB(n) * 1000;
}

inline constexpr auto
ZB(auto n)
{
    return EB(n) * 1000;
}

inline constexpr auto
YB(auto n)
{
    return ZB(n) * 1000;
}

inline constexpr auto
operator""_KiB(unsigned long long n) // ???
{
    return KiB(n);
}
