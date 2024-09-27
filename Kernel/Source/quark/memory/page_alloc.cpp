#include <quark/api/memory.h>
#include <quark/memory/memory_info.h>
#include <quark/memory/page_alloc.h>

#include <mixins/math/align.h>
#include <mixins/math/compute.h>

namespace Quark::System::Memory {
    Array<AddressRange[256]>                         g_pageRanges;
    Array<PhysMemQueue[BUDDY_LEVEL_UPPER_LIMIT + 1]> g_pageQueues;
    PhysMemFrame**                                   g_pageFrames;
    // PhysMemFrame**               g_pageFrames;
}

namespace Quark::System::API {
    using namespace Quark::System::Memory;

    // General Memory Allocation and Free

    Res<u64> allocMemory4K(usize                amount,
                           AddressSpace*        addressSpace,
                           Flags<Hal::VmmFlags> flags)
    {
        Res<u64> phys = ::allocPhysMemory4K(amount);

        if (phys.isOkay()) {
            u64 address = 0;

            addressSpace->Map4KPages(
                phys.unwrap(),
                address =
                    ::allocVirtMemory4K(amount, addressSpace, flags).unwrap(),
                amount,
                flags);
            return Ok(address);
        }

        // Call page swapping here

        return Error::OutOfMemory();
    }

    Res<> freeMemory4K(usize address, usize amount, AddressSpace* addressSpace)
    {
        if (!address || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        Res<u64> phys = addressSpace->GetPhysAddress(address);

        if (phys.isOkay()) {
            freePhysMemory4K(phys.unwrap(), amount);
        }

        freeVirtMemory4K(address, amount, addressSpace);
        return Ok();
    }

    Res<> mapAddress(u64                  phys,
                     u64                  virt,
                     usize                amount,
                     AddressSpace*        addressSpace,
                     Flags<Hal::VmmFlags> flags)
    {
        if (!phys || !virt || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        addressSpace->Map4KPages(phys, virt, amount, flags);

        return Ok();
    }

    // Physical Memory Management

    Res<PhysMemFrame*> allocPhysFrame4K(usize amount)
    {
        if (amount == 0 || (amount % PAGE_SIZE_4K) != 0) {
            return Error::AllocateFailed("Invalid allocation size");
        }

        alignTwoExponent(amount);

        /* Loop search for the level with free page */
        u8 level = 0;
        while (level < BUDDY_LEVEL_UPPER_LIMIT) {
            if ((1 << level) >= amount && g_pageQueues[level].count())
                break;
            level++;
        }

        /* If no page is found, return out of memory error */
        if (level > BUDDY_LEVEL_UPPER_LIMIT || !g_pageQueues[level].count()) {
            return Error::OutOfMemory();
        }

        PhysMemFrame* p = g_pageQueues[level].dequeue().Take();
        while ((1 << level) > amount && level > 0) {
            level--;
            g_pageQueues[level].enqueue(p->split().Take());
        }

        p->_flags.clear(Hal::PmmFlags::FREE);

        return Ok(p);
    }

    Res<u64> allocPhysMemory4K(usize amount)
    {
        return allocPhysFrame4K(amount).map<u64>(
            [](PhysMemFrame* frame) { return frame->_address; });
    }

    Res<> freePhysMemory4K(usize address, usize amount)
    {
        PhysMemFrame* page = PhysMemFrame::at(address);

        /* Ensure the page is okay to be free */
        if (page->_flags & (Hal::PmmFlags::FREE |   //
                            Hal::PmmFlags::KERNEL | //
                            Hal::PmmFlags::SWAPPED)) {
            return Error::PageNotFree();
        }

        /* Mark the page as free */
        page->_flags |= Hal::PmmFlags::FREE;
        while (page->_level < BUDDY_LEVEL_UPPER_LIMIT) {
            /*
                Merge the page until it reaches the upper bound
                or the another page needed is not free, then jump
                out of the loop
             */
            u64           offset  = GetLevelAsOffset(page->_level);
            u64           newAddr = (page->_address % (1 << (offset * 2)))
                                        ? page->_address + offset
                                        : page->_address - offset;
            PhysMemFrame* newPage = PhysMemFrame::at(newAddr);

            if (newPage->_flags.has(Hal::PmmFlags::FREE)) {
                if (newPage->_previous || newPage->_next) {
                    g_pageQueues[newPage->_level].dequeue(newPage);
                }

                PhysMemFrame* result = page->merge(newPage).Take();
                if (result != nullptr) {
                    page = result;
                    continue;
                }
            } else
                break;
        }
        g_pageQueues[page->_level].enqueue(page);

        return Ok();
    }
    // Virtual Memory Management

    Res<u64> allocVirtMemory4K(usize                amount,
                               AddressSpace*        addressSpace,
                               Flags<Hal::VmmFlags> flags)
    {
        if (!amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        return addressSpace->Alloc4KPages(amount, flags);
    }

    Res<> freeVirtMemory4K(usize         address,
                           usize         amount,
                           AddressSpace* addressSpace)
    {
        if (!address || !amount || !addressSpace) {
            return Error::InvalidArgument();
        }

        return addressSpace->Free4KPages(address, amount);
    }
}