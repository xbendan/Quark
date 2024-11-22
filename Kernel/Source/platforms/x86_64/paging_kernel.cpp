#include <platforms/x86_64/address_space.h>
#include <platforms/x86_64/paging.h>
#include <quark/memory/address_range.h>
#include <quark/memory/page.h>
#include <quark/os/main.h>

#define COMPUTE_PHYS_ADDR(addr) ((u64)(addr) - KERNEL_VIRTUAL_BASE)

namespace Quark::System::Platform::X64 {
    using Quark::System::Memory::AddressRange;

    X64AddressSpace<UserManagement::Level::System> kAddressSpace;

    pml4_t    kPml4 __attribute__((aligned(PAGE_SIZE_4K)));     // 0x249000
    pdpt_t    kPdpt __attribute__((aligned(PAGE_SIZE_4K)));     // 0x248000
    pagedir_t kPageDirs __attribute__((aligned(PAGE_SIZE_4K))); // 0x247000
    pagedir_t kHeapDirs __attribute__((aligned(PAGE_SIZE_4K)));
    pagetbl_t kPageTbls[512] __attribute__((aligned(PAGE_SIZE_4K)));
    pagetbl_t kHeapTbls[512] __attribute__((aligned(PAGE_SIZE_4K)));
    pagedir_t kIoDirs[4] __attribute__((aligned(PAGE_SIZE_4K)));

    X64AddressSpace<UserManagement::Level::System>::X64AddressSpace()
    {
        AddressRange(&kPml4, sizeof(pml4_t)).SetZero();
        AddressRange(&kPdpt, sizeof(pdpt_t)).SetZero();

        SetPageFrame(kPml4[KERNEL_BASE_PML4_INDEX],
                     COMPUTE_PHYS_ADDR(&kPdpt),
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        kPml4[0] = kPml4[KERNEL_BASE_PML4_INDEX];

        // Map kernel base
        SetPageFrame(kPdpt[KERNEL_BASE_PDPT_INDEX],
                     COMPUTE_PHYS_ADDR(&kPageDirs),
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            SetPageFrame(kPageDirs[i],
                         PAGE_SIZE_2M * i,
                         VmmFlags::PRESENT | VmmFlags::WRITABLE |
                             VmmFlags::PAGE_SIZE | VmmFlags::WRITE_THROUGH);
        }
        _pageDirs[0] = &kPageDirs;

        // Map kernel heap
        SetPageFrame(kPdpt[KERNEL_HEAP_PDPT_INDEX],
                     COMPUTE_PHYS_ADDR(&kHeapDirs),
                     VmmFlags::PRESENT | VmmFlags::WRITABLE);
        for (int i = 0; i < VMM_PAGE_ENTRY_COUNT; i++) {
            SetPageFrame(kHeapDirs[i],
                         COMPUTE_PHYS_ADDR((u64)&kHeapTbls + PAGE_SIZE_4K * i),
                         VmmFlags::PRESENT | VmmFlags::WRITABLE);
        }

        // Set IO tables
        for (int i = 0; i < 4; i++) {
            SetPageFrame(kPdpt[PDPT_GET_INDEX(IO_VIRTUAL_BASE) + i],
                         COMPUTE_PHYS_ADDR(&kIoDirs[i]),
                         VmmFlags::PRESENT | VmmFlags::WRITABLE);
            for (int j = 0; j < VMM_PAGE_ENTRY_COUNT; j++) {
                SetPageFrame(kIoDirs[i][j],
                             PAGE_SIZE_1G * i + PAGE_SIZE_2M * j,
                             VmmFlags::PRESENT | VmmFlags::WRITABLE |
                                 VmmFlags::PAGE_SIZE | VmmFlags::WRITE_THROUGH);
            }
        }

        kPdpt[0]  = kPdpt[KERNEL_BASE_PDPT_INDEX];
        _pml4Phys = COMPUTE_PHYS_ADDR(&kPml4);

        asm volatile("mov %%rax, %%cr3" ::"a"(_pml4Phys));
    }

    Res<u64>
    X64AddressSpace<UserManagement::Level::System>::AllocateVirtPages4K(
        usize           amount,
        Flags<VmmFlags> flags)
    {
        u64 offset = 0, pageDirOffset = 0;
        u64 count   = 0;
        u64 address = 0;

        flags |= (VmmFlags::PRESENT);

        for (int i = 0; i < TABLES_PER_DIR; i++) {
            for (int j = 0; j < PAGES_PER_TABLE; j++) {
                if (kHeapTbls[i][j] & VmmFlags::PRESENT) {
                    pageDirOffset = i;
                    offset        = j + 1;
                    count         = 0;
                    continue;
                }

                count++;
                if (count >= amount) {
                    address = ((pageDirOffset * PAGE_SIZE_2M) +
                               (offset * PAGE_SIZE_4K)) |
                              KERNEL_HEAP_BASE;
                    while (count--) {
                        if (offset >= PAGES_PER_TABLE) {
                            pageDirOffset++;
                            offset = 0;
                        }
                        SetPageFrame(
                            kHeapTbls[pageDirOffset][offset], 0, flags);
                        offset++;
                    }

                    return Ok(address);
                }
            }
        }

        return Error::OutOfMemory();
    }

    Res<u64>
    X64AddressSpace<UserManagement::Level::System>::AllocateVirtPages2M(
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<UserManagement::Level::System>::FreeVirtPages4K(
        u64   address,
        usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<UserManagement::Level::System>::FreeVirtPages2M(
        u64   address,
        usize amount)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<UserManagement::Level::System>::MapAddress4K(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        if (PDPT_GET_INDEX(virt) != KERNEL_HEAP_PDPT_INDEX) {
            return Error::InvalidArgument();
        }

        u64 pageDirIndex, pageIndex;
        while (amount--) {
            pageDirIndex = PDIR_GET_INDEX(virt);
            pageIndex    = PAGE_GET_INDEX(virt);
            SetPageFrame(kHeapTbls[pageDirIndex][pageIndex],
                         phys,
                         flags | VmmFlags::PRESENT);
            phys += PAGE_SIZE_4K;
            virt += PAGE_SIZE_4K;
        }
        return Ok();
    }

    Res<> X64AddressSpace<UserManagement::Level::System>::MapAddress2M(
        u64             phys, //
        u64             virt,
        usize           amount,
        Flags<VmmFlags> flags)
    {
        return Error::NotImplemented();
    }

    Res<Flags<VmmFlags>>
    X64AddressSpace<UserManagement::Level::System>::getFlags(u64 address)
    {
        return Error::NotImplemented();
    }

    Res<> X64AddressSpace<UserManagement::Level::System>::setFlags(
        u64             address,
        Flags<VmmFlags> flags,
        bool            set)
    {
        return Error::NotImplemented();
    }

    Res<u64> X64AddressSpace<UserManagement::Level::System>::GetPhysAddress(
        u64 address)
    {
        return Error::NotImplemented();
    }
}

namespace Quark::System {
    using Quark::System::Memory::AddressSpace;
    using namespace Quark::System::Platform::X64;

    Res<AddressSpace*> InitVirtMemory()
    {
        AddressSpace* p = &kAddressSpace;
        if (!kAddressSpace._pml4Phys) {
            new (p) X64AddressSpace<UserManagement::Level::System>();
        }
        return Ok(p);
    }

    namespace Memory {
        Res<AddressSpace*> AddressSpace::Create(UserManagement::Level lv)
        {
            return Ok(static_cast<AddressSpace*>(
                new X64AddressSpace<UserManagement::Level::User>()));
        }

        u64 CopyAsIOAddress(u64 address)
        {
            return address + IO_VIRTUAL_BASE;
        }
    }
}