#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <mixins/utils/bitmap.h>
#include <mixins/utils/flags.h>

#define VMM_PAGE_ADDRESS_MASK 0x0000fffffffff000
#define VMM_PAGE_ENTRY_COUNT 512

#define PML4_GET_INDEX(addr) ((addr >> 39) & 0x1FF)
#define PDPT_GET_INDEX(addr) ((addr >> 30) & 0x1FF)
#define PDIR_GET_INDEX(addr) ((addr >> 21) & 0x1FF)
#define PAGE_GET_INDEX(addr) ((addr >> 12) & 0x1FF)
#define PAGE_OFFSET_MASK 0x0000000000000FFF

#define PAGES_PER_TABLE 512
#define TABLES_PER_DIR 512
#define DIRS_PER_PDPT 512
#define PDPTS_PER_PML4 512

#define VMM_PAGE_PRESENT (1 << 0)
#define VMM_PAGE_WRITABLE (1 << 1)
#define VMM_PAGE_USER (1 << 2)
#define VMM_PAGE_WRITE_THROUGH (1 << 3)
#define VMM_PAGE_CACHE_DISABLED (1 << 4)
#define VMM_PAGE_ACCESSED (1 << 5)
#define VMM_PAGE_DIRTY (1 << 6)
#define VMM_PAGE_PAGE_SIZE (1 << 7)
#define VMM_PAGE_GLOBAL (1 << 7)
#define VMM_PAGE_DISABLE_EXECUTE (1 << 11)

#define KERNEL_BASE_PML4_INDEX PML4_GET_INDEX(KERNEL_VIRTUAL_BASE)
#define KERNEL_BASE_PDPT_INDEX PDPT_GET_INDEX(KERNEL_VIRTUAL_BASE)
#define KERNEL_HEAP_PDPT_INDEX 511

namespace Quark::System::Platform::X64 {
    using pageflags_t = u64;
    using pml4_t      = pageflags_t[VMM_PAGE_ENTRY_COUNT];
    using pdpt_t      = pageflags_t[VMM_PAGE_ENTRY_COUNT];
    using pagedir_t   = pageflags_t[VMM_PAGE_ENTRY_COUNT];
    using pagetbl_t   = pageflags_t[VMM_PAGE_ENTRY_COUNT];

    enum class VmmFlags : u64
    {
        PRESENT         = (1 << 0),
        WRITABLE        = (1 << 1),
        USER            = (1 << 2),
        WRITE_THROUGH   = (1 << 3),
        CACHE_DISABLED  = (1 << 4),
        ACCESSED        = (1 << 5),
        DIRTY           = (1 << 6),
        PAGE_SIZE       = (1 << 7),
        GLOBAL          = (1 << 7),
        DISABLE_EXECUTE = (1 << 11),
    };
    MakeFlags$(VmmFlags);

    constexpr inline void SetPageFrame(pageflags_t& pf, u64 address, u64 flags)
    {
        pf = (pf & ~VMM_PAGE_ADDRESS_MASK) | (address & VMM_PAGE_ADDRESS_MASK) |
             (flags & ~VMM_PAGE_ADDRESS_MASK);
    }

    inline void SetPageFrame(pageflags_t&    pf,
                             u64             address,
                             Flags<VmmFlags> flags)
    {
        u64 _flags = (flags() & ~VMM_PAGE_ADDRESS_MASK) |
                     (address & VMM_PAGE_ADDRESS_MASK);
        pf = _flags;
    }

    inline void SetPageFlags(pageflags_t&    pf,
                             Flags<VmmFlags> flags,
                             bool            set = true)
    {
        u64 _flags = flags() & ~VMM_PAGE_ADDRESS_MASK;
        pf         = set ? (pf | _flags) : (pf & ~_flags);
    }

    template <typename T>
    inline T* AllocatePageTable()
    {
        return (T*)new pageflags_t[VMM_PAGE_ENTRY_COUNT];
    }

    inline bool operator&(pageflags_t flags, VmmFlags flag)
    {
        return flags & static_cast<u64>(flag);
    }
}