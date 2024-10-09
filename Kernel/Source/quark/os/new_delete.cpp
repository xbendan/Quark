#include <mixins/std/c++types.h>
#include <quark/memory/page_alloc.h>

void*
operator new(usize size)
{
    return (void*)Quark::System::Memory::Allocate(size).Unwrap();
}

void*
operator new[](usize size)
{
    return (void*)Quark::System::Memory::Allocate(size).Unwrap();
}

void
operator delete(void* p) noexcept
{
    Quark::System::Memory::Free((u64)p);
}

void
operator delete(void* p, unsigned long)
{
    Quark::System::Memory::Free((u64)p);
}

void
operator delete[](void* p) noexcept
{
    Quark::System::Memory::Free((u64)p);
}

void
operator delete[](void* p, unsigned long)
{
    Quark::System::Memory::Free((u64)p);
}