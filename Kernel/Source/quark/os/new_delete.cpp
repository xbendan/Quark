#include <mixins/std/c++types.h>
#include <mixins/std/panic.h>
#include <quark/memory/page_alloc.h>

void*
operator new(usize size)
{
    auto res = Quark::System::Memory::Allocate(size);
    if (res.IsError()) {
        // Std::panic("Failed to allocate memory: {}", res.Err().message());
        Std::panic("Failed to allocate memory");
    }
    return (void*)res.Unwrap();
}

void*
operator new[](usize size)
{
    auto res = Quark::System::Memory::Allocate(size);
    if (res.IsError()) {
        // Std::panic("Failed to allocate memory: {}", res.Err().message());
        Std::panic("Failed to allocate memory");
    }
    return (void*)res.Unwrap();
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