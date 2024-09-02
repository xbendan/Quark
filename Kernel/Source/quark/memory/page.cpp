#include <quark/api/logging.h>
#include <quark/api/memory.h>
#include <quark/init/boot_info.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>
#include <quark/memory/slab.h>
#include <quark/os/main.h>

#include <mixins/math/compute.h>

namespace Quark {
    using namespace Quark::Mem;

    Res<> initPhysMemory()
    {
        OsMemoryConfig& info = getBootInfo()._memory;

        // Copy memory ranges and initialize a temporary page allocator
        for (u8 i = 0, j = 0; i < info._addressRanges.size(); i++) {
            if (info._addressRanges[i]._type ==
                OsMemRangeType::MEM_RANGE_TYPE_FREE)
                (g_pageRanges[j++] = info._addressRanges[i]._value)
                    .innerAlign(PAGE_SIZE_4K);
        }
        auto allocPhysBlock4K = [](usize amount) -> u64 {
            alignUpRef(amount, PAGE_SIZE_4K);
            Opt<u64> address = 0;
            for (usize i = 0; i < 256; i++) {
                if ((address = g_pageRanges[i].takeFront(amount)).isPresent())
                    return address.take();
            }
            return 0;
        };

        // Reserve space for page frame structures
        u64 const pages      = PAGE_SIZE_PARTITION / PAGE_SIZE_4K;
        u64       partitions = divCeil(info._limit, PAGE_SIZE_PARTITION);

        g_pageFrames = (Mem::PhysMemFrame**)allocPhysBlock4K(
            partitions * sizeof(Mem::PhysMemFrame**));
        for (int i = 0; i < partitions; i++) {
            g_pageFrames[i] = (Mem::PhysMemFrame*)allocPhysBlock4K(
                pages * sizeof(Mem::PhysMemFrame*));
        }

        // Load slub allocator
        for (int i = 0; i < SLAB_CACHE_BLOCK_AMOUNT; i++) {
            g_slabCaches[i] = new (
                (void*)::allocPhysMemory4K(sizeof(Mem::SlabCache)).unwrap())
                Mem::SlabCache(g_slabAmounts[i], 0);
        }

        return Ok();
    }
}

namespace Quark::Mem {

    Opt<PhysMemFrame*> PhysMemFrame::split()
    {
        if (_level == 0 || not _flags.has(Hal::PmmFlags::FREE)) {
            return Empty();
        }

        _level--;

        u64 offset = getLevelAsOffset(_level, sizeof(PhysMemFrame));
        // PhysMemFrame* page   = (PhysMemFrame*)(this + offset);

        // *page = {
        //     ._level   = _level,
        //     ._flags   = _flags,
        //     ._priv    = _priv,
        //     ._address = _address,
        // };

        return new (this + offset) PhysMemFrame{
            ._level   = _level,
            ._flags   = _flags,
            ._priv    = _priv,
            ._address = _address + offset,
        };
    }

    Opt<PhysMemFrame*> PhysMemFrame::merge(PhysMemFrame* page)
    {
        if (/* Ensure both pages are free and can be merged */
            !(_flags.has(Hal::PmmFlags::FREE) &&
              page->_flags.has(Hal::PmmFlags::FREE)) //
            ||
            /* Ensure these pages are connected in address */
            (abs(_address - page->_address) != (getLevelAsOffset(_level)))) {
            return nullptr;
        }

        PhysMemFrame* result = (_address % (getLevelAsOffset(_level) * 2)) //
                                   ? this
                                   : page;
        result->_level++;

        return result;
    }

    Opt<PhysMemFrame*> PhysMemFrame::merge()
    {
        u64 offset = getLevelAsOffset(_level);
        if (!isPageAligned(_address, _level + 1)) {
            offset = -offset;
        }

        return merge((PhysMemFrame*)(this + offset));
    }

    PhysMemFrame* PhysMemFrame::at(u64 address)
    {
        return (PhysMemFrame*)(g_pageFrames + (address / PAGE_SIZE_4K));
    }
} // namespace Quark::Mem