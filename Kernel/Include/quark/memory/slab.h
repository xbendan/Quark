#include <mixins/concurrent/spinlock.h>
#include <mixins/str/string.h>
#include <mixins/utils/linked_list.h>
#include <quark/hal/multiprocessing.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>
#include <quark/memory/page_queue.h>

#define SLAB_CACHE_BLOCK_AMOUNT 15

namespace Quark::System::Memory {
    using Quark::System::Hal::CPULocal;

    struct SlabAllocNode
    {
        /*
         * Pointer to the next object, this is a double pointer
         * because the area this pointer point to is also a pointer
         * to the next object.
         */
        // void**     _freelist;
        PageFrame* _page;
        PageQueue  _partial;

        void RefillIfEmpty();
        u64  Alloc();
    };
    using kmem_alloc_node_t = SlabAllocNode;

    struct SlabCache
    {
        constexpr SlabCache(u64 size, u64 flags = 0)
            : _size(size)
            , _objSize(size)
            , _objAlign(0)
            , _reserved(PAGE_SIZE_4K % size)
        {
        }
        ~SlabCache() = default;

        void RefillNodeIfEmpty(kmem_alloc_node_t* node);
        u64  Take(kmem_alloc_node_t* node);
        // void InitPageAsCached(PageFrame* page, u64 address);

        Qk::StringView              _name;
        // struct Node
        // {
        //     Spinlock  _spinlock;
        //     u64       _nr;
        //     PageQueue _partial;
        // } _nodes[4];
        CPULocal<kmem_alloc_node_t> _cpus;
        kmem_alloc_node_t           _fallback;
        lock_t                      _lock;
        u32                         _size;
        u32                         _objSize;
        u32                         _objAlign;
        u32                         _offset;
        u64                         _minPartial;
        u32                         _reserved;
    };
    using kmem_cache_t = SlabCache;

    // extern usize        g_slabAmounts[SLAB_CACHE_BLOCK_AMOUNT];
    extern kmem_cache_t g_slabCaches[SLAB_CACHE_BLOCK_AMOUNT];
}