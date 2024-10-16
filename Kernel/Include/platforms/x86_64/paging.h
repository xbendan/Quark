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
}

namespace Quark::System::Platform::X64 {

    // template <u8 _Lv>
    // class MapLevel
    // {
    // public:
    //     union Entry
    //     {
    //         u64 _flags;

    //         struct
    //         {
    //             u64 _present : 1;
    //             u64 _writable : 1;
    //             u64 _user : 1;
    //             u64 _writeThrough : 1;
    //             u64 _cacheDisabled : 1;
    //             u64 _accessed : 1;
    //             u64 _dirty : 1;
    //             u64 _pageSize : 1;
    //             u64 _global : 1;
    //             u64 __reserved__2 : 3;
    //             u64 _address : 40;
    //             u64 __reserved__3 : 11;
    //             u64 _disableExecute : 1;
    //         } __attribute__((packed));

    //         Entry& WithFlags(VmmFlags flags, bool set = true)
    //         {
    //             switch (flags) {
    //                 case VmmFlags::PRESENT:
    //                     _present = set;
    //                     break;
    //                 case VmmFlags::WRITABLE:
    //                     _writable = set;
    //                     break;
    //                 case VmmFlags::USER:
    //                     _user = set;
    //                     break;
    //                 case VmmFlags::WRITE_THROUGH:
    //                     _writeThrough = set;
    //                     break;
    //                 case VmmFlags::CACHE_DISABLED:
    //                     _cacheDisabled = set;
    //                     break;
    //                 case VmmFlags::ACCESSED:
    //                     _accessed = set;
    //                     break;
    //                 case VmmFlags::DIRTY:
    //                     _dirty = set;
    //                     break;
    //                 case VmmFlags::PAGE_SIZE:
    //                     _pageSize = set;
    //                     break;
    //                 case VmmFlags::GLOBAL:
    //                     _global = set;
    //                     break;
    //                 case VmmFlags::DISABLE_EXECUTE:
    //                     _disableExecute = set;
    //                     break;
    //             }
    //             return *this;
    //         }

    //         Entry& withFlags(Flags<VmmFlags> flags, bool set = true)
    //         {
    //             _present        = flags & VmmFlags::PRESENT;
    //             _writable       = flags & VmmFlags::WRITABLE;
    //             _user           = flags & VmmFlags::USER;
    //             _writeThrough   = flags & VmmFlags::WRITE_THROUGH;
    //             _cacheDisabled  = flags & VmmFlags::CACHE_DISABLED;
    //             _accessed       = flags & VmmFlags::ACCESSED;
    //             _dirty          = flags & VmmFlags::DIRTY;
    //             _pageSize       = flags & VmmFlags::PAGE_SIZE;
    //             _global         = flags & VmmFlags::GLOBAL;
    //             _disableExecute = flags & VmmFlags::DISABLE_EXECUTE;

    //             return *this;
    //         }

    //         Entry& clearFlags(Flags<VmmFlags> flags)
    //         {
    //             _present        = !(flags & VmmFlags::PRESENT);
    //             _writable       = !(flags & VmmFlags::WRITABLE);
    //             _user           = !(flags & VmmFlags::USER);
    //             _writeThrough   = !(flags & VmmFlags::WRITE_THROUGH);
    //             _cacheDisabled  = !(flags & VmmFlags::CACHE_DISABLED);
    //             _accessed       = !(flags & VmmFlags::ACCESSED);
    //             _dirty          = !(flags & VmmFlags::DIRTY);
    //             _pageSize       = !(flags & VmmFlags::PAGE_SIZE);
    //             _global         = !(flags & VmmFlags::GLOBAL);
    //             _disableExecute = !(flags & VmmFlags::DISABLE_EXECUTE);

    //             return *this;
    //         }

    //         Entry& WithAddress(u64 address)
    //         {
    //             _address = address & VMM_PAGE_ADDRESS_MASK;
    //             return *this;
    //         }

    //     } __attribute__((packed)) _entries[VMM_PAGE_ENTRY_COUNT];

    //     static_assert(sizeof(MapLevel<_Lv>::Entry) == 8);

    //     constexpr static usize _len = 512;

    //     Entry& operator[](usize index) { return _entries[index]; }

    //     const Entry& operator[](usize index) const { return _entries[index];
    //     }
    // };

    // using Pml4      = MapLevel<4>; // Page Map Level 4, 256 TiB
    // using Pdpt      = MapLevel<3>; // Page Directory Pointer Table, 512 GiB
    // using PageDir   = MapLevel<2>; // Page Directory, 1 GiB
    // using PageTable = MapLevel<1>; // Page Table, 2 MiB

    // inline constexpr u16 pml4IndexOf(u64 addr)
    // {
    //     return (addr >> 39) & 0x1FF;
    // }

    // inline constexpr u16 pdptIndexOf(u64 addr)
    // {
    //     return (addr >> 30) & 0x1FF;
    // }

    // inline constexpr u16 pdirIndexOf(u64 addr)
    // {
    //     return (addr >> 21) & 0x1FF;
    // }

    // inline constexpr u16 pageIndexOf(u64 addr)
    // {
    //     return (addr >> 12) & 0x1FF;
    // }

} // namespace Quark::System::Platform::X64