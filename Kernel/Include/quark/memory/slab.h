#include <mixins/concurrent/spinlock.h>
#include <mixins/std/string.h>
#include <mixins/utils/linked_list.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>

#define SLAB_CACHE_BLOCK_AMOUNT 15

namespace Quark::System::Memory {
    struct SlabCpuCache
    {
        /*
         * Pointer to the next object, this is a double pointer
         * because the area this pointer point to is also a pointer
         * to the next object.
         */
        void**        _freelist;
        PhysMemFrame* _page;
        PhysMemQueue  _partial;
    };

    struct SlabCache
    {
        SlabCache(u64 size, u64 flags)
            : _size(size)
            , _objSize(size)
            , _objAlign(0)
            , _reserved(PAGE_SIZE_4K % size)
        {
        }
        ~SlabCache() = delete;

        void pageInitAsCached(PhysMemFrame* page, u64 address);

        string _name;
        struct Node
        {
            Spinlock     _spinlock;
            u64          _nr;
            PhysMemQueue _partial;
        } _nodes[4];
        SlabCpuCache _unit[256];
        u32          _size;
        // Flags<>      _flags;
        u32          _objSize;
        u32          _objAlign;
        u32          _offset;
        u64          _minPartial;
        u32          _reserved;
    };

    extern usize      g_slabAmounts[SLAB_CACHE_BLOCK_AMOUNT];
    extern SlabCache* g_slabCaches[SLAB_CACHE_BLOCK_AMOUNT];
}