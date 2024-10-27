#include <platforms/x86_64/address_space.h>
#include <platforms/x86_64/paging.h>

#include <quark/memory/address_range.h>
#include <quark/memory/page.h>

#define PAGE_FLAGS_PRU (VmmFlags::PRESENT | VmmFlags::WRITABLE | VmmFlags::USER)
#define PAGE_FLAGS_PR (VmmFlags::PRESENT | VmmFlags::WRITABLE)

namespace Quark::System::Platform::X64 {
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

    Res<u64> X64AddressSpace<Privilege::Level::User>::AllocateVirtPages4K(
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

    Res<u64> X64AddressSpace<Privilege::Level::User>::AllocateVirtPages2M(
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::FreeVirtPages4K(u64 address,
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

    Res<> X64AddressSpace<Privilege::Level::User>::FreeVirtPages2M(u64 address,
                                                                   usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::MapAddress4K(
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

    Res<> X64AddressSpace<Privilege::Level::User>::MapAddress2M(
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
} // namespace Quark::System::Platform::X64
