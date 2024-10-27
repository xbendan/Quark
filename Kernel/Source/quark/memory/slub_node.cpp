#include <quark/init/boot_info.h>
#include <quark/memory/page_alloc.h>
#include <quark/memory/slab.h>

namespace Quark::System::Memory {
    void kmem_cache_t::RefillNodeIfEmpty(kmem_alloc_node_t* node)
    {
        if (node->_page &&
            (node->_page->_kmem._inuse < node->_page->_kmem._objects)) {
            return;
        }

        auto* addressSpace = BootInfo::MemoryInfo._addressSpace;
        auto* page         = Memory::AllocatePhysFrame4K(1).Unwrap();

        u64 address = addressSpace->AllocateVirtPages4K(1).Unwrap();

        page->_flags |= Hal::PmmFlags::KERNEL;
        page->_kmemPool = this;
        page->_freelist = (void**)address;

        page->_kmem._objects = PAGE_SIZE_4K / _size;
        page->_kmem._inuse   = 0;

        MapAddress(page->_address,
                   address,
                   1,
                   addressSpace,
                   (Hal::VmmFlags::PRESENT | Hal::VmmFlags::WRITABLE));

        for (int i = 0; i < page->_kmem._objects - 1; i++)
            *((u64*)(address + i * _size)) = (u64)(address + (i + 1) * _size);

        node->_page     = page;
        page->_freelist = (void**)address;
        // node->_freelist = (void**)address;
    }
}