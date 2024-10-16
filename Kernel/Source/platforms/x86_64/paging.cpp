#include <platforms/x86_64/address_space.h>
#include <platforms/x86_64/paging.h>

#include <quark/memory/address_range.h>
#include <quark/memory/page.h>

#define PAGE_FLAGS_PRU (VmmFlags::PRESENT | VmmFlags::WRITABLE | VmmFlags::USER)
#define PAGE_FLAGS_PR (VmmFlags::PRESENT | VmmFlags::WRITABLE)

namespace Quark::System::Platform::X64 {
    using Quark::System::Memory::AddressRange;

    pagedir_t kPageDirs;
    pagedir_t kIoDirs[4];
    pagedir_t kHeapDirs;
    pagetbl_t kHeapTbls[512];

    X64AddressSpace<Privilege::Level::System> kAddressSpace;

    X64AddressSpace<Privilege::Level::User>::X64AddressSpace()
        : _pml4Phys((u64)&_pml4 - KERNEL_VIRTUAL_BASE)
        , _bitmap(new Bitmap(1'6384 / 8 * 1024))
    {
        SetPageFrame(
            _pml4[0], (u64)&_pdpt - KERNEL_VIRTUAL_BASE, PAGE_FLAGS_PRU);
        SetPageFrame(
            _pdpt[0], (u64)&_pageDirs[0] - KERNEL_VIRTUAL_BASE, PAGE_FLAGS_PRU);
    }

    X64AddressSpace<Privilege::Level::User>::~X64AddressSpace()
    {
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            if (!_pageDirs[i]) {
                break;
            }
            delete[] _pageDirs[i];
            if (!_pageTables[i]) {
                continue;
            }
            for (int j = 0; j < VMM_PAGE_ENTRY_COUNT; j++) {
                if (!_pageTables[i][j]) {
                    break;
                }
                delete[] _pageTables[i][j];
            }
        }
        delete _bitmap;
        // Do we delete all the page tables?
        // I hate nested pointers.
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::Alloc4KPages(
        usize           amount, //
        Flags<VmmFlags> flags)
    {
        u64 index = _bitmap->find(amount);
        u16 i, j, k;
        while (amount) {
            i = (index >> 18) & 0x1ff;
            j = (index >> 9) & 0x1ff;
            k = index & 0x1ff;
            if (!_pageDirs[i]) {
                _pageDirs[i] = reinterpret_cast<pagedir_t*>(
                    new pageflags_t[VMM_PAGE_ENTRY_COUNT]);
                _pageTables[i] = new pagetbl_t*[VMM_PAGE_ENTRY_COUNT];

                SetPageFrame(
                    _pdpt[i], (u64)&_pageDirs[i] - KERNEL_VIRTUAL_BASE, flags);
            }

            if (_pageTables[i][j] == nullptr) {
                _pageTables[i][j] = reinterpret_cast<pagetbl_t*>(
                    new pageflags_t[VMM_PAGE_ENTRY_COUNT]);

                SetPageFrame((*_pageDirs[i])[j],
                             (u64)&_pageTables[i][j] - KERNEL_VIRTUAL_BASE,
                             flags);
            }

            SetPageFrame((*_pageTables[i][j])[k], (u64)_zeroPage, flags);

            amount--;
            index++;
        }
        return Ok(index << 12);
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::Alloc2MPages(
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::Free4KPages(u64   address,
                                                               usize amount)
    {
        u64 index = address >> 12;
        u16 i, j, k;
        while (amount) {
            i = (index >> 18) & 0x1ff;
            j = (index >> 9) & 0x1ff;
            k = index & 0x1ff;

            if (!_pageTables[i] || !_pageTables[i][j]) {
                continue;
            }

            SetPageFrame(
                (*_pageTables[i][j])[k], (u64)_zeroPage, PAGE_FLAGS_PRU);

            amount--;
            index++;
        }
        return Ok();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::Free2MPages(u64   address,
                                                               usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::Map4KPages(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        u64 index = virt >> 12;
        u16 i, j, k;
        while (amount--) {
            i = (index >> 18) & 0x1ff;
            j = (index >> 9) & 0x1ff;
            k = index & 0x1ff;

            if (!(_pdpt[i] & VMM_PAGE_PRESENT)) {
                if (_pageDirs[i])
                    return Error::PageMayBroken();

                _pageDirs[i]   = AllocatePageTable<pagedir_t>();
                _pageTables[i] = new pagetbl_t*[VMM_PAGE_ENTRY_COUNT];
                SetPageFrame(_pdpt[i],
                             (u64)&_pageDirs[i] - KERNEL_VIRTUAL_BASE,
                             flags | VmmFlags::PRESENT);
            }

            if (!((*_pageDirs[i])[j] & VMM_PAGE_PRESENT)) {
                if (_pageTables[i][j])
                    return Error::PageMayBroken();

                _pageTables[i][j] = AllocatePageTable<pagetbl_t>();
                SetPageFrame((*_pageDirs[i])[j],
                             (u64)&_pageTables[i][j] - KERNEL_VIRTUAL_BASE,
                             flags | VmmFlags::PRESENT);
            }

            SetPageFrame(
                (*_pageTables[i][j])[k], phys, flags | VmmFlags::PRESENT);

            index++;
            phys += PAGE_SIZE_4K;
            virt += PAGE_SIZE_4K;
        }
        return Ok();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::Map2MPages(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<Flags<VmmFlags>> X64AddressSpace<Privilege::Level::User>::getFlags(
        u64 address)
    {
        u64 index = address >> 12;
        u16 i, j, k;
        i = (index >> 18) & 0x1ff;
        j = (index >> 9) & 0x1ff;
        k = index & 0x1ff;

        if (!_pageDirs[i] || !_pageTables[i][j])
            return Error::PageMayBroken();

        pageflags_t     pf = (*_pageTables[i][j])[k];
        Flags<VmmFlags> flags;
        // clang-format off
        if (pf & VMM_PAGE_PRESENT) flags |= VmmFlags::PRESENT;
        if (pf & VMM_PAGE_WRITABLE) flags |= VmmFlags::WRITABLE;
        if (pf & VMM_PAGE_USER) flags |= VmmFlags::USER;
        if (pf & VMM_PAGE_WRITE_THROUGH) flags |= VmmFlags::WRITE_THROUGH;
        if (pf & VMM_PAGE_CACHE_DISABLED) flags |= VmmFlags::CACHE_DISABLED;
        if (pf & VMM_PAGE_ACCESSED) flags |= VmmFlags::ACCESSED;
        if (pf & VMM_PAGE_DIRTY) flags |= VmmFlags::DIRTY;
        if (pf & VMM_PAGE_PAGE_SIZE) flags |= VmmFlags::PAGE_SIZE;
        if (pf & VMM_PAGE_GLOBAL) flags |= VmmFlags::GLOBAL;
        if (pf & VMM_PAGE_DISABLE_EXECUTE) flags |= VmmFlags::DISABLE_EXECUTE;
        // clang-format on

        return Ok(flags);
    }

    Res<> X64AddressSpace<Privilege::Level::User>::setFlags(
        u64             address,
        Flags<VmmFlags> flags,
        bool            set)
    {
        u64 index = address >> 12;
        u16 i, j, k;
        i = (index >> 18) & 0x1ff;
        j = (index >> 9) & 0x1ff;
        k = index & 0x1ff;

        if (!_pageDirs[i] || !_pageTables[i][j])
            return Error::PageMayBroken();

        pageflags_t& pf = (*_pageTables[i][j])[k];
        SetPageFlags(pf, flags, set);

        return Ok();
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::GetPhysAddress(
        u64 address)
    {
        // u64 index = address >> 12;
        // u16 i, j, k;
        // i = (index >> 18) & 0x1ff;
        // j = (index >> 9) & 0x1ff;
        // k = index & 0x1ff;

        // if (!_pageDirs[i] || !_pageTables[i][j])
        //     return Error::PageMayBroken();

        // return Ok((*_pageTables[i][j])[k]._address);

        return Error::NotImplemented();
    }

    // Res<Flags<VmmFlags>> X64AddressSpace<
    //     Privilege::Level::User>::setFlagsThenGet(u64 address,
    //                                          Flags<VmmFlags> flags,
    //                                          bool                 set)
    // {
    //     u64 index = address >> 12;
    //     u16 i, j, k;
    //     i = (index >> 18) & 0x1ff;
    //     j = (index >> 9) & 0x1ff;
    //     k = index & 0x1ff;

    //     if (!_pageDirs[i] || !_pageTables[i][j])
    //         return Error::PageMayBroken();

    //     MapLevel<1>::Entry& ent = (*_pageTables[i][j])[k];
    //     if (set) {
    //         ent.withFlags(flags);
    //     } else {
    //         ent.clearFlags(flags);
    //     }

    //     Flags<VmmFlags> newFlags;
    //     if (ent._present) {
    //         newFlags |= VmmFlags::PRESENT;
    //     }
    //     if (ent._writable) {
    //         newFlags |= VmmFlags::WRITABLE;
    //     }
    //     if (ent._user) {
    //         newFlags |= VmmFlags::USER;
    //     }
    //     if (ent._accessed) {
    //         newFlags |= VmmFlags::ACCESSED;
    //     }
    //     if (ent._dirty) {
    //         newFlags |= VmmFlags::DIRTY;
    //     }
    //     if (ent._pageSize) {
    //         newFlags |= VmmFlags::PAGE_SIZE;
    //     }
    //     if (ent._global) {
    //         newFlags |= VmmFlags::GLOBAL;
    //     }
    //     if (ent._disableExecute) {
    //         newFlags |= VmmFlags::DISABLE_EXECUTE;
    //     }
    //     return Ok(newFlags);
    // }

    X64AddressSpace<Privilege::Level::System>::X64AddressSpace()
    {
        AddressRange(&_pml4, sizeof(pml4_t)).Clear();
        AddressRange(&_pdpt, sizeof(pdpt_t)).Clear();

        SetPageFrame(_pml4[KERNEL_BASE_PML4_INDEX],
                     (u64)&_pdpt - KERNEL_VIRTUAL_BASE,
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        _pml4[0] = _pml4[KERNEL_BASE_PML4_INDEX];

        // Map kernel base
        SetPageFrame(_pdpt[KERNEL_BASE_PDPT_INDEX],
                     (u64)&kPageDirs - KERNEL_VIRTUAL_BASE,
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            SetPageFrame(kPageDirs[i],
                         PAGE_SIZE_2M * i,
                         VmmFlags::PRESENT | VmmFlags::WRITABLE);
        }
        _pageDirs[0] = &kPageDirs;

        // Map kernel heap
        SetPageFrame(_pdpt[KERNEL_HEAP_PDPT_INDEX],
                     (u64)&kHeapDirs - KERNEL_VIRTUAL_BASE,
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            SetPageFrame(kHeapDirs[i],
                         (u64)&kHeapTbls - KERNEL_VIRTUAL_BASE +
                             PAGE_SIZE_4K * i,
                         VmmFlags::PRESENT | VmmFlags::WRITABLE);
        }

        // Set IO tables
        for (int i = 0; i < 4; i++) {
            SetPageFrame(_pdpt[PDPT_GET_INDEX(IO_VIRTUAL_BASE) + i],
                         (u64)&kIoDirs[i] - KERNEL_VIRTUAL_BASE,
                         VmmFlags::PRESENT | VmmFlags::WRITABLE);
            for (int j = 0; j < VMM_PAGE_ENTRY_COUNT; j++) {
                SetPageFrame(kIoDirs[i][j],
                             PAGE_SIZE_1G * i + PAGE_SIZE_2M * j,
                             VmmFlags::PRESENT | VmmFlags::WRITABLE);
            }
        }

        _pdpt[0]  = _pdpt[KERNEL_BASE_PDPT_INDEX];
        _pml4Phys = (u64)&_pml4 - KERNEL_VIRTUAL_BASE;

        // asm("mov %%rax, %%cr3" ::"a"(_pml4Phys));
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::Alloc4KPages(
        usize           amount,
        Flags<VmmFlags> flags)
    {

        return Error::NotImplemented();
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::Alloc2MPages(
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::Free4KPages(u64   address,
                                                                 usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::Free2MPages(u64   address,
                                                                 usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::Map4KPages(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::Map2MPages(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<Flags<VmmFlags>> X64AddressSpace<Privilege::Level::System>::getFlags(
        u64 address)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::setFlags(
        u64             address,
        Flags<VmmFlags> flags,
        bool            set)
    {
        return Error::NotImplemented();
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::GetPhysAddress(
        u64 address)
    {
        return Error::NotImplemented();
    }
} // namespace Quark::System::Platform::X64

namespace Quark::System {
    using Quark::System::Memory::AddressSpace;
    using namespace Quark::System::Platform::X64;

    Res<AddressSpace*> InitVirtMemory()
    {
        AddressSpace* p = &kAddressSpace;
        if (!kAddressSpace._pml4Phys) {
            new (p) X64AddressSpace<Privilege::Level::System>();
        }
        return Ok(p);
    }
}

namespace Quark::System::Memory {
    Res<AddressSpace*> AddressSpace::Create(Privilege::Level lv)
    {
        return Ok((AddressSpace*)new X64AddressSpace<Privilege::Level::User>());
    }

    u64 CopyAsIOAddress(u64 address)
    {
        return address + IO_VIRTUAL_BASE;
    }
}