#include <platforms/x86_64/paging.h>

#include <quark/memory/address_range.h>
#include <quark/memory/page.h>

namespace Quark::Platform::X64 {

    PageDir   kPageDirs;
    PageDir   kIoDirs[4];
    PageDir   kHeapDirs;
    PageTable kHeapTbls[512];

    X64AddressSpace<Privilege::Level::System> kAddressSpace;

    X64AddressSpace<Privilege::Level::User>::X64AddressSpace()
        : _pml4Phys((u64)&_pml4 - KERNEL_VIRTUAL_BASE)
        , _bitmap(new Bitmap(1'6384 / 8 * 1024))
    {
        _pml4[0]
            .withFlags(Hal::VmmFlags::PRESENT |  //
                       Hal::VmmFlags::WRITABLE | //
                       Hal::VmmFlags::USER)
            .withAddress((u64)&_pdpt - KERNEL_VIRTUAL_BASE);
        _pdpt[0]
            .withFlags(Hal::VmmFlags::PRESENT |  //
                       Hal::VmmFlags::WRITABLE | //
                       Hal::VmmFlags::USER)
            .withAddress((u64)&_pageDirs[0] - KERNEL_VIRTUAL_BASE);
    }

    X64AddressSpace<Privilege::Level::User>::~X64AddressSpace()
    {
        for (int i = 0; i < PageDir::_len; i++) {
            if (!_pageDirs[i]) {
                break;
            }
            delete _pageDirs[i];
            if (!_pageTables[i]) {
                continue;
            }
            for (int j = 0; j < PageTable::_len; j++) {
                if (!_pageTables[i][j]) {
                    break;
                }
                delete _pageTables[i][j];
            }
        }
        delete _bitmap;
        // Do we delete all the page tables?
        // I hate nested pointers.
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::alloc4KPages(
        usize                amount, //
        Flags<Hal::VmmFlags> flags)
    {
        u64 index = _bitmap->find(amount);
        u16 i, j, k;
        while (amount) {
            i = (index >> 18) & 0x1ff;
            j = (index >> 9) & 0x1ff;
            k = index & 0x1ff;
            if (!_pageDirs[i]) {
                _pageDirs[i]   = new MapLevel<2>();
                _pageTables[i] = new MapLevel<1>*[VMM_PAGE_ENTRY_COUNT];

                _pdpt[i] //
                    .withFlags(flags)
                    .withAddress((u64)&_pageDirs[i] - KERNEL_VIRTUAL_BASE);
            }

            if (_pageTables[i][j] == nullptr) {
                _pageTables[i][j] = new MapLevel<1>();

                (*_pageDirs[i])[j] //
                    .withFlags(flags)
                    .withAddress((u64)&_pageTables[i][j] - KERNEL_VIRTUAL_BASE);
            }

            (*_pageTables[i][j])[k]           //
                .withFlags(flags)             //
                .withAddress((u64)_zeroPage); //

            amount--;
            index++;
        }
        return Ok(index << 12);
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::alloc2MPages(
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::free4KPages(u64   address,
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

            (*_pageTables[i][j])[k] //
                .withFlags(Hal::VmmFlags::PRESENT)
                .withAddress((u64)_zeroPage);

            amount--;
            index++;
        }
        return Ok();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::free2MPages(u64   address,
                                                               usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::map4KPages(
        u64                  phys, //
        u64                  virt,
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        u64 index = virt >> 12;
        u16 i, j, k;
        while (amount--) {
            i = (index >> 18) & 0x1ff;
            j = (index >> 9) & 0x1ff;
            k = index & 0x1ff;

            if (!_pdpt[i]._present) {
                if (_pageDirs[i])
                    return Error::PageMayBroken();

                _pageDirs[i]   = new MapLevel<2>();
                _pageTables[i] = new MapLevel<1>*[VMM_PAGE_ENTRY_COUNT];
                _pdpt[i]
                    .withFlags(flags)
                    .withFlags(Hal::VmmFlags::PRESENT)
                    .withAddress((u64)&_pageDirs[i] - KERNEL_VIRTUAL_BASE);
            }

            if (!(*_pageDirs[i])[j]._present) {
                if (_pageTables[i][j])
                    return Error::PageMayBroken();

                _pageTables[i][j] = new MapLevel<1>();
                (*_pageDirs[i])[j]
                    .withFlags(flags)
                    .withFlags(Hal::VmmFlags::PRESENT)
                    .withAddress((u64)&_pageTables[i][j] - KERNEL_VIRTUAL_BASE);
            }

            (*_pageTables[i][j])[k]
                .withFlags(flags)
                .withFlags(Hal::VmmFlags::PRESENT)
                .withAddress(phys);

            index++;
            phys += PAGE_SIZE_4K;
            virt += PAGE_SIZE_4K;
        }
        return Ok();
    }

    Res<> X64AddressSpace<Privilege::Level::User>::map2MPages(
        u64                  phys, //
        u64                  virt,
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<Flags<Hal::VmmFlags>> X64AddressSpace<Privilege::Level::User>::getFlags(
        u64 address)
    {
        u64 index = address >> 12;
        u16 i, j, k;
        i = (index >> 18) & 0x1ff;
        j = (index >> 9) & 0x1ff;
        k = index & 0x1ff;

        if (!_pageDirs[i] || !_pageTables[i][j])
            return Error::PageMayBroken();

        MapLevel<1>::Entry&  ent = (*_pageTables[i][j])[k];
        Flags<Hal::VmmFlags> flags;
        if (ent._present) {
            flags |= Hal::VmmFlags::PRESENT;
        }
        if (ent._writable) {
            flags |= Hal::VmmFlags::WRITABLE;
        }
        if (ent._user) {
            flags |= Hal::VmmFlags::USER;
        }
        if (ent._accessed) {
            flags |= Hal::VmmFlags::ACCESSED;
        }
        if (ent._dirty) {
            flags |= Hal::VmmFlags::DIRTY;
        }
        if (ent._pageSize) {
            flags |= Hal::VmmFlags::PAGE_SIZE;
        }
        if (ent._global) {
            flags |= Hal::VmmFlags::GLOBAL;
        }
        if (ent._disableExecute) {
            flags |= Hal::VmmFlags::DISABLE_EXECUTE;
        }
        return Ok(flags);
    }

    Res<> X64AddressSpace<Privilege::Level::User>::setFlags(
        u64                  address,
        Flags<Hal::VmmFlags> flags,
        bool                 set)
    {
        u64 index = address >> 12;
        u16 i, j, k;
        i = (index >> 18) & 0x1ff;
        j = (index >> 9) & 0x1ff;
        k = index & 0x1ff;

        if (!_pageDirs[i] || !_pageTables[i][j])
            return Error::PageMayBroken();

        MapLevel<1>::Entry& ent = (*_pageTables[i][j])[k];
        if (set) {
            ent.withFlags(flags);
        } else {
            ent.clearFlags(flags);
        }

        return Ok();
    }

    Res<u64> X64AddressSpace<Privilege::Level::User>::getPhysAddress(
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

    // Res<Flags<Hal::VmmFlags>> X64AddressSpace<
    //     Privilege::Level::User>::setFlagsThenGet(u64 address,
    //                                          Flags<Hal::VmmFlags> flags,
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

    //     Flags<Hal::VmmFlags> newFlags;
    //     if (ent._present) {
    //         newFlags |= Hal::VmmFlags::PRESENT;
    //     }
    //     if (ent._writable) {
    //         newFlags |= Hal::VmmFlags::WRITABLE;
    //     }
    //     if (ent._user) {
    //         newFlags |= Hal::VmmFlags::USER;
    //     }
    //     if (ent._accessed) {
    //         newFlags |= Hal::VmmFlags::ACCESSED;
    //     }
    //     if (ent._dirty) {
    //         newFlags |= Hal::VmmFlags::DIRTY;
    //     }
    //     if (ent._pageSize) {
    //         newFlags |= Hal::VmmFlags::PAGE_SIZE;
    //     }
    //     if (ent._global) {
    //         newFlags |= Hal::VmmFlags::GLOBAL;
    //     }
    //     if (ent._disableExecute) {
    //         newFlags |= Hal::VmmFlags::DISABLE_EXECUTE;
    //     }
    //     return Ok(newFlags);
    // }

    X64AddressSpace<Privilege::Level::System>::X64AddressSpace()
    {
        AddressRange(&_pml4, sizeof(Pml4)).clear();
        AddressRange(&_pdpt, sizeof(Pdpt)).clear();

        _pml4[pml4IndexOf(KERNEL_VIRTUAL_BASE)]
            .withFlags(Hal::VmmFlags::PRESENT | //
                       Hal::VmmFlags::WRITABLE)
            .withAddress((u64)&_pdpt - KERNEL_VIRTUAL_BASE);
        _pml4[0] = _pml4[pml4IndexOf(KERNEL_VIRTUAL_BASE)];

        _pdpt[pdptIndexOf(KERNEL_VIRTUAL_BASE)]
            .withFlags(Hal::VmmFlags::PRESENT | //
                       Hal::VmmFlags::WRITABLE)
            .withAddress((u64)&kPageDirs - KERNEL_VIRTUAL_BASE);
        _pageDirs[0] = &kPageDirs;

        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            kPageDirs[i]
                .withFlags(Hal::VmmFlags::PRESENT |        //
                           Hal::VmmFlags::WRITABLE |       //
                           Hal::VmmFlags::CACHE_DISABLED | //
                           Hal::VmmFlags::PAGE_SIZE)
                .withAddress((u64)&kHeapTbls - KERNEL_VIRTUAL_BASE);
        }

        // Set kernel heap tables
        _pdpt[VMM_PAGE_ENTRY_COUNT - 1]
            .withFlags(Hal::VmmFlags::PRESENT | //
                       Hal::VmmFlags::WRITABLE)
            .withAddress((u64)&kHeapDirs - KERNEL_VIRTUAL_BASE);
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            kHeapDirs[i]
                .withFlags(Hal::VmmFlags::PRESENT | //
                           Hal::VmmFlags::WRITABLE)
                .withAddress((u64)&kHeapTbls - KERNEL_VIRTUAL_BASE);
        }

        // Set IO tables
        for (int i = 0; i < 4; i++) {
            _pdpt[pdptIndexOf(IO_VIRTUAL_BASE) + i]
                .withFlags(Hal::VmmFlags::PRESENT | //
                           Hal::VmmFlags::WRITABLE)
                .withAddress((u64)&kIoDirs[i] - KERNEL_VIRTUAL_BASE);
            for (int j = 0; j < VMM_PAGE_ENTRY_COUNT; j++) {
                kIoDirs[i][j]
                    .withFlags(Hal::VmmFlags::PRESENT | //
                               Hal::VmmFlags::WRITABLE)
                    .withAddress((u64)(PAGE_SIZE_1G * i + PAGE_SIZE_2M * j));
            }
        }

        _pdpt[0]  = _pdpt[pdptIndexOf(KERNEL_VIRTUAL_BASE)];
        _pml4Phys = (u64)&_pml4 - KERNEL_VIRTUAL_BASE;
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::alloc4KPages(
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {

        return Error::NotImplemented();
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::alloc2MPages(
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::free4KPages(u64   address,
                                                                 usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::free2MPages(u64   address,
                                                                 usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::map4KPages(
        u64                  phys, //
        u64                  virt,
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::map2MPages(
        u64                  phys, //
        u64                  virt,
        usize                amount,
        Flags<Hal::VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<Flags<Hal::VmmFlags>>
    X64AddressSpace<Privilege::Level::System>::getFlags(u64 address)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<Privilege::Level::System>::setFlags(
        u64                  address,
        Flags<Hal::VmmFlags> flags,
        bool                 set)
    {
        return Error::NotImplemented();
    }

    Res<u64> X64AddressSpace<Privilege::Level::System>::getPhysAddress(
        u64 address)
    {
        return Error::NotImplemented();
    }
} // namespace Quark::Platform::X64

namespace Quark {
    using Quark::Mem::AddressSpace;
    using namespace Quark::Platform::X64;

    Res<AddressSpace*> initVirtMemory()
    {
        AddressSpace* p = &kAddressSpace;
        if (!kAddressSpace._pml4Phys) {
            new (p) X64AddressSpace<Privilege::Level::System>();
        }
        return Ok(p);
    }

    namespace API {
        Res<AddressSpace*> createAddressSpace(Privilege::Level lv)
        {
            return Ok(
                (AddressSpace*)new X64AddressSpace<Privilege::Level::User>());
        }
    }
}