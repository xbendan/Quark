#pragma once

#ifndef __hosted__

inline void*
operator new(long unsigned int size, void* p) noexcept
{
    return p;
}

inline void*
operator new[](long unsigned int size, void* p) noexcept
{
    return p;
}

#endif