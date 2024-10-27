#include <mixins/std/assert.h>
#include <quark/memory/buddy_system.h>

namespace Quark::System::Memory {
    Res<PageFrame*> BuddyZone::Allocate4KPages(usize amount)
    {
        if (amount == 0 || amount > PAGE_CHAIN_AMOUNT) {
            return Error::AllocateFailed("Invalid allocation size");
        }

        ScopedLock lock(m_lock);
        Math::AlignTwoExponent(amount);

        u8 pageQueueLevel = 0;
        while (pageQueueLevel < BUDDY_LEVEL_UPPER_LIMIT) {
            if ((1 << pageQueueLevel) >= amount &&
                m_pageQueues[pageQueueLevel].Count())
                break;
            pageQueueLevel++;
        }

        /* If no page is found, return out of memory error */
        if (pageQueueLevel > BUDDY_LEVEL_UPPER_LIMIT ||
            !m_pageQueues[pageQueueLevel].Count()) {
            return Error::OutOfMemory();
        }

        PageFrame* page = m_pageQueues[pageQueueLevel].Dequeue();
        while ((1 << pageQueueLevel) > amount && pageQueueLevel) {
            pageQueueLevel--;
            auto rest = page->Divide();
            if (!rest.IsPresent()) {
                Std::SystemPanic("Failed to divide page, it's not free");
            }

            m_pageQueues[pageQueueLevel].Enqueue(rest.Take());
        }

        page->_flags.Unset(Hal::PmmFlags::FREE);
        m_managedPages.FetchSub(amount);

        return Ok(page);
    };

    Res<> BuddyZone::Free4KPages(PageFrame* page)
    {
        if (!page) {
            return Error::NullReference();
        }
        if (m_range.WithinRange(page->_address)) {
            return Error::PageNotExist();
        }

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
            u64        offset  = GetLevelAsOffset(page->_level);
            u64        newAddr = (page->_address % (1 << (offset * 2)))
                                     ? page->_address + offset
                                     : page->_address - offset;
            PageFrame* newPage = PageFrame::ByAddress(newAddr);

            if (newPage->_flags.has(Hal::PmmFlags::FREE)) {
                if (newPage->_previous || newPage->_next) {
                    m_pageQueues[newPage->_level].Dequeue(newPage);
                }

                PageFrame* result = page->Combine(newPage).Take();
                if (result != nullptr) {
                    page = result;
                    continue;
                }
            } else
                break;
        }
        m_pageQueues[page->_level].Enqueue(page);

        return Ok();
    }

    Res<> BuddyZone::Free4KPages(u64 address)
    {
        PageFrame* page = PageFrame::ByAddress(address);
        return Free4KPages(page);
    }

    void BuddyZone::MarkRegionAsUsed(AddressRange range)
    {
        MakeAssertion(range.From() && range.To(), "Invalid address range");
        auto& pageRange = range
                              .InnerAlign(PAGE_SIZE_4K) //
                              .ConstraintsTo(m_range);
        while (pageRange.From() < PAGE_BIOS_RESERVED) {
            /* To avoid potential issues, don't mark memory below 0x100000 */
            pageRange._min += PAGE_SIZE_4K;
        }
        ScopedLock lock(m_lock);

        u64 address = range.From();
        while (address < range.To()) {
            auto* pageFrame = PageFrame::ByAddress(address);

            if (!(pageFrame->_flags & Hal::PmmFlags::FREE)) {
                continue;
            }

            pageFrame->_flags.Unset(Hal::PmmFlags::FREE);
            if (pageFrame->_previous || pageFrame->_next) {
                m_pageQueues[pageFrame->_level].Dequeue(pageFrame);
                m_managedPages.Dec();
            }

            address += PAGE_SIZE_4K;
        }
    }

    void BuddyZone::MarkRegionAsFree(AddressRange range)
    {
        MakeAssertion(range.From() && range.To(), "Invalid address range");
        auto& pageRange = range
                              .InnerAlign(PAGE_SIZE_4K) //
                              .ConstraintsTo(m_range);
        while (pageRange.From() < PAGE_BIOS_RESERVED)
            /* To avoid potential issues, don't mark memory below 0x100000
             */
            pageRange._min += PAGE_SIZE_4K;
        ScopedLock lock(m_lock);

        usize      pageLevelAssumption = BUDDY_LEVEL_UPPER_LIMIT;
        PageFrame* pageFrame           = nullptr;

        usize amount = pageRange.Length() / PAGE_SIZE_4K;

        m_presentPages += amount;
        m_managedPages.FetchAdd(amount);

        u64 address = range.From();
        u64 offset  = 0;
        while (address < range.To()) {
            while ((offset = GetLevelAsOffset(pageLevelAssumption)) >
                   (range.To() - address)) {
                pageLevelAssumption--;
            }

            pageFrame = PageFrame::ByAddress(address);
            if (!pageFrame->_address &&
                !(pageFrame->_flags & Hal::PmmFlags::FREE)) {
                for (int i = 0; i < (1 << pageLevelAssumption); i++)
                    pageFrame[i] = {
                        ._flags    = Hal::PmmFlags::FREE,
                        ._level    = 0,
                        ._chainLength = 1,
                        ._kmem     = {
                            ._inuse   = 0,
                            ._objects = 0,
                            ._frozen  = 0,
                        },
                        ._pageHead = pageFrame,
                        ._address  = address + (i * PAGE_SIZE_4K),
                    };
            }
            pageFrame->_level = pageLevelAssumption;

            m_pageQueues[pageLevelAssumption].Enqueue(pageFrame);

            address += offset;
        }
    }
}