#include <quark/hal/multiprocessing.h>
#include <quark/init/boot_info.h>
#include <quark/memory/address_space.h>
#include <quark/memory/slab.h>
#include <quark/os/main.h>

#include <mixins/math/compute.h>

namespace Quark::System::Memory {
    __attribute__((used,
                   section(".data"))) //
    SlabCache g_slabCaches[SLAB_CACHE_BLOCK_AMOUNT] = {
        (8),   (16),  (24),  (32),   (48),   (64),   (96),   (128),
        (256), (384), (512), (1024), (1536), (2048), (4096),
    };

    // void SlabCache::InitPageAsCached(PageFrame* page, u64 address)
    // {
    //     page->_flags |= Hal::PmmFlags::KERNEL;
    //     page->_kmemPool = this;
    //     page->_freelist = (void**)address;

    //     page->_kmem._objects = (PAGE_SIZE_4K - sizeof(PageFrame)) / _size;
    //     page->_kmem._inuse   = 0;

    //     for (int i = 0; i < page->_kmem._objects - 1; i++)
    //         *((u64*)(address + i * _size)) = (u64)(address + (i + 1) *
    //         _size);
    // }

    u64 kmem_cache_t::Take(kmem_alloc_node_t* node)
    {
        RefillNodeIfEmpty(node);

        auto* page = node->_page;

        u64 address     = (u64)page->_freelist;
        page->_freelist = (void**)*page->_freelist;

        if (++page->_kmem._inuse == page->_kmem._objects) {
            node->_page = nullptr;
        }

        AddressRange(address, _size).SetZero();
        return address;
    }

    Res<u64> Allocate(usize amount)
    {
        if (amount > PAGE_SIZE_4K) {
            return AllocateMemory4K(DivCeil(amount, PAGE_SIZE_4K),
                                    BootInfo::MemoryInfo._addressSpace,
                                    Hal::VmmFlags::WRITABLE);
            // return AllocatePhysMemory4K(DivCeil(amount, PAGE_SIZE_4K));
        }
        kmem_cache_t* cache = nullptr;
        for (usize i = 0; i < SLAB_CACHE_BLOCK_AMOUNT; i++)
            if (amount <= g_slabCaches[i]._size) {
                cache = &g_slabCaches[i];
                break;
            }
        if (!cache) {
            return Error::OutOfMemory();
        }

        u64   address  = 0;
        /*
         * Fastest path, get object from per cpu cache,
         * if the cache is empty, allocate a new page,
         * no need to lock, but disable preemption.
         */
        auto* cpuLocal = cache->_cpus();

        if (cpuLocal) {
            address = cache->Take(cpuLocal);
        } else {
            ScopedLock<true> lock(cache->_lock);
            address = cache->Take(&cache->_fallback);
        }

        if (!address) {
            return Error::OutOfMemory();
        }

        return Ok(address);
    }

    /**
     * @brief
     *
     * @param address The virtual address to be freed
     * @return Res<>
     */
    Res<> Free(u64 address)
    {
        auto* addressSpace = Process::GetKernelProcess()->_addressSpace;
        auto  result       = addressSpace->GetPhysAddress(address);

        if (result.isError()) {
            return result.Err();
        }

        auto* page = PageFrame::ByAddress(result.unwrap());
        assert(page != nullptr, "Invalid page frame for address");

        if (page->_kmemPool == nullptr ||
            !(page->_flags & Hal::PmmFlags::KERNEL))
        /* This means the page didn't mark as kmem, try free with buddy */ {
            return FreePhysFrame4K(page);
        }

        kmem_cache_t* cache = reinterpret_cast<kmem_cache_t*>(page->_kmemPool);
        kmem_alloc_node_t* node = cache->_cpus();

        if (!node) {
            node = &cache->_fallback;
        }

        assert(page->_kmem._inuse != page->_kmem._objects,
               "Invalid \"inuse\" count in page {}");

        *((u64*)address) = (u64)page->_freelist;
        page->_freelist  = (void**)address;
        // TODO: check the node freelist

        if (++page->_kmem._inuse == 1) {
            node->_partial.Enqueue(page);
        } else {
        }

        return Ok();
    }
}