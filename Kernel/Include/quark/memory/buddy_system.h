#include <mixins/std/c++types.h>

#include <mixins/concurrent/atomic.h>
#include <mixins/concurrent/spinlock.h>
#include <quark/memory/address_range.h>
#include <quark/memory/page_queue.h>

#define BUDDY_LEVEL_UPPER_LIMIT 10
#define BUDDY_LEVEL_LOWER_LIMIT 0
#define BUDDY_ZONE_DMA_LIMIT 0x1000000     // 16 MiB
#define BUDDY_ZONE_DMA32_LIMIT 0x100000000 // 4 GiB
#define BUDDY_ZONE_NORMAL_LIMIT -1         // Unlimited

namespace Quark::System::Memory {

    enum ZoneType
    {
        ZONE_DMA,
        ZONE_DMA32,
        ZONE_NORMAL,
        ZONE_HIGHMEM,

        ZONE_TYPES
    };

    /**
     * @brief
     * @ref Linux mmzone.h
     * https://github.com/torvalds/linux/blob/master/include/linux/mmzone.h
     */
    class BuddyZone
    {
    public:
        BuddyZone() = default;
        BuddyZone(AddressRange range)
            : m_range(range.InnerAlign(PAGE_SIZE_4K))
            , m_pfnStart(range.From() >> PAGE_SHIFT)
            , m_pfnEnd(range.To() >> PAGE_SHIFT)
            , m_spannedPages(range.Length() >> PAGE_SHIFT)
            , m_managedPages(0)
            , m_presentPages(0)
        {
            for (int i = BUDDY_LEVEL_LOWER_LIMIT; i <= BUDDY_LEVEL_UPPER_LIMIT;
                 i++) {
                m_pageQueues[i] = PageQueue();
            }
        }
        BuddyZone(u64 start, u64 end)
            : BuddyZone(AddressRange(start, end))
        {
        }

        AddressRange GetRange() const { return m_range; }

        Res<PageFrame*> Allocate4KPages(usize amount);
        Res<>           Free4KPages(PageFrame* pageframe);
        Res<>           Free4KPages(u64 address);
        void            MarkRegionAsUsed(AddressRange);
        void            MarkRegionAsFree(AddressRange);

    private:
        AddressRange m_range;
        u64          m_pfnStart;
        u64          m_pfnEnd;
        /*
         * spanned_pages is the total pages spanned by the zone, including
         * holes, which is calculated as:
         * 	spanned_pages = zone_end_pfn - zone_start_pfn;
         */
        u64          m_spannedPages;
        /*
         * managed_pages is present pages managed by the buddy system, which
         * is calculated as (reserved_pages includes pages allocated by the
         * bootmem allocator):
         *	managed_pages = present_pages - reserved_pages;
         */
        Atomic<long> m_managedPages;
        /*
         * present_pages is physical pages existing within the zone, which
         * is calculated as:
         *	present_pages = spanned_pages - absent_pages(pages in holes);
         *
         * So present_pages may be used by memory hotplug or memory power
         * management logic to figure out unmanaged pages by checking
         * (present_pages - managed_pages). And managed_pages should be used
         * by page allocator and vm scanner to calculate all kinds of watermarks
         * and thresholds.
         */
        u64          m_presentPages;

        PageQueue m_pageQueues[BUDDY_LEVEL_UPPER_LIMIT + 1];
        /*
         * Locking rules:
         *
         * zone_start_pfn and spanned_pages are protected by span_seqlock.
         * It is a seqlock because it has to be read outside of zone->lock,
         * and it is done in the main allocator path.  But, it is written
         * quite infrequently.
         *
         * The span_seq lock is declared along with zone->lock because it is
         * frequently read in proximity to zone->lock.  It's good to
         * give them a chance of being in the same cacheline.
         *
         * Write access to present_pages at runtime should be protected by
         * mem_hotplug_begin/done(). Any reader who can't tolerant drift of
         * present_pages should use get_online_mems() to get a stable value.
         */
        lock_t    m_lock;
    };
}