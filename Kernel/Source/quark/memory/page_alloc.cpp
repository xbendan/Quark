#include <quark/memory/address_space.h>
#include <quark/memory/buddy_system.h>
#include <quark/memory/memory_info.h>
#include <quark/memory/page_alloc.h>

#include <mixins/math/align.h>
#include <mixins/math/compute.h>

namespace Quark::System::Memory {
    PageFrame** g_pageFrames;
    // PhysMemFrame**               g_pageFrames;

    BuddyZone buddyZones[ZONE_TYPES];

    // General Memory Allocation and Free

    Res<u64> AllocateMemory4K(usize                amount,
                              AddressSpace*        addressSpace,
                              Flags<Hal::VmmFlags> flags)
    {
        Res<PageFrame*> phys = AllocatePhysFrame4K(amount);

        if (phys.IsOkay()) {
            u64 address = 0;

            addressSpace->MapAddress4K(
                phys.Unwrap()->_address,
                address =
                    AllocateVirtMemory4K(amount, addressSpace, flags).Unwrap(),
                amount,
                flags);
            return Ok(address);
        }

        // Call page swapping here

        return Error::OutOfMemory();
    }

    Res<> FreeMemory4K(usize address, usize amount, AddressSpace* addressSpace)
    {
        if (!address || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        Res<u64> phys = addressSpace->GetPhysAddress(address);

        if (phys.IsOkay()) {
            FreePhysMemory4K(phys.Unwrap(), amount);
        }

        FreeVirtMemory4K(address, amount, addressSpace);
        return Ok();
    }

    Res<> MapAddress(u64                  phys,
                     u64                  virt,
                     usize                amount,
                     AddressSpace*        addressSpace,
                     Flags<Hal::VmmFlags> flags)
    {
        if (!phys || !virt || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        addressSpace->MapAddress4K(phys, virt, amount, flags);

        return Ok();
    }

    // Physical Memory Management

    Res<PageFrame*> AllocatePhysFrame4K(usize amount, Hal::PmmType allocType)
    {
        u8 t = static_cast<u8>(allocType);

        if (t > static_cast<u8>(Hal::PmmType::NORMAL)) {
            return Error::AllocateFailed("Invalid allocation type");
        }
        while (t >= 1) {
            Res<PageFrame*> res = buddyZones[t - 1].Allocate4KPages(amount);
            if (res.IsOkay()) {
                return Ok(res.Unwrap());
            }
            t--;
        };
        return Error::OutOfMemory("Failed to allocate memory");
    }

    Res<u64> AllocatePhysMemory4K(usize amount)
    {
        auto res = AllocatePhysFrame4K(amount);
        if (res.IsError()) {
            return res.Err();
        }
        return Ok(res.Unwrap()->_address);

        // return AllocatePhysFrame4K(amount).Select<u64>(
        //     [](PhysMemFrame* frame) { return frame->_address; });
    }

    Res<> FreePhysFrame4K(PageFrame* page)
    {
        return FreePhysMemory4K(page->_address, page->_chainLength);
    }

    Res<> FreePhysMemory4K(u64 address, usize amount)
    {
        for (int i = 0; i < ZONE_TYPES; i++) {
            if (buddyZones[i].GetRange().WithinRange(address))
                return buddyZones[i].Free4KPages(address);
        }
        return Error::PageNotExist();
    }
    // Virtual Memory Management

    Res<u64> AllocateVirtMemory4K(usize                amount,
                                  AddressSpace*        addressSpace,
                                  Flags<Hal::VmmFlags> flags)
    {
        if (!amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        return addressSpace->AllocateVirtPages4K(amount, flags);
    }

    Res<> FreeVirtMemory4K(usize         address,
                           usize         amount,
                           AddressSpace* addressSpace)
    {
        if (!address || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        return addressSpace->FreeVirtPages4K(address, amount);
    }
}