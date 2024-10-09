#include <quark/memory/address_space.h>
#include <quark/memory/slab.h>
#include <quark/os/main.h>

#include <mixins/math/compute.h>

namespace Quark::System::Memory {
    usize g_slabAmounts[] = {
        8,   16,  24,  32,   48,   64,   96,   128, //
        256, 384, 512, 1024, 1536, 2048, 4096,
    };
    SlabCache* g_slabCaches[SLAB_CACHE_BLOCK_AMOUNT];

    void SlabCache::pageInitAsCached(PhysMemFrame* page, u64 address)
    {
        page->_flags |= Hal::PmmFlags::KERNEL;
        page->_kmemPool = this;
        page->_freelist = (void**)address;

        page->_objects = (PAGE_SIZE_4K - sizeof(PhysMemFrame)) / _size;
        page->_inuse   = 0;

        for (int i = 0; i < page->_objects - 1; i++)
            *((u64*)(address + i * _size)) = (u64)(address + (i + 1) * _size);
    }

    Res<u64> Allocate(usize amount)
    {
        if (amount > PAGE_SIZE_4K) {
            return AllocatePhysMemory4K(divCeil(amount, PAGE_SIZE_4K));
        }
        SlabCache* cache = nullptr;
        for (usize i = 0; i < SLAB_CACHE_BLOCK_AMOUNT; i++)
            if (amount <= g_slabAmounts[i]) {
                cache = g_slabCaches[i];
                break;
            }
        if (!cache) {
            return Error::OutOfMemory();
        }

        // Change 0 to current CPU ID
        SlabCpuCache* cpu  = &cache->_unit[0];
        PhysMemFrame* page = cpu->_page;
        u64           address;
        if (!page || page->_inuse == page->_objects) {
            page = cpu->_page = AllocatePhysFrame4K(1).Unwrap();

            cache->pageInitAsCached(
                page,
                address = AllocateVirtMemory4K(
                              1, Process::GetKernelProcess()->_addressSpace)
                              .Unwrap());
            MapAddress(page->_address, address, 1, nullptr, 0);
        } else
            address = (u64)page->_freelist;

        page->_freelist = (void**)*page->_freelist;
        /* Check whether the objects in this page is running out */
        if (++page->_inuse == page->_objects) {
            cpu->_page = nullptr;
        }

        AddressRange(address, cache->_size).clear();
        return Ok(address);
    }

    Res<> Free(u64 address)
    {
        return Ok();
    }
}