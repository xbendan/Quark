#include <mixins/std/c++types.h>
#include <quark/api/memory.h>

using namespace Quark::System::API;

void*
operator new(usize size)
{
    return (void*)alloc(size).unwrap();
}

void*
operator new[](usize size)
{
    return (void*)alloc(size).unwrap();
}

void
operator delete(void* p) noexcept
{
    free((u64)p);
}

void
operator delete(void* p, unsigned long)
{
    free((u64)p);
}

void
operator delete[](void* p) noexcept
{
    free((u64)p);
}

void
operator delete[](void* p, unsigned long)
{
    free((u64)p);
}