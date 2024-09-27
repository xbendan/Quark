#include <quark/api/logging.h>
#include <quark/api/memory.h>
#include <quark/init/boot_info.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>
#include <quark/memory/slab.h>
#include <quark/os/main.h>

#include <mixins/math/compute.h>

namespace Quark::System {
    using namespace Quark::System::Memory;

    Res<> InitPhysMemory()
    {
        MemoryConfiguration& info = getLaunchConfiguration()._memory;

        // Copy memory ranges and initialize a temporary page allocator
        for (u8 i = 0, j = 0; i < info._addressRanges.size(); i++) {
            if (info._addressRanges[i]._type == MemmapEntry::Free)
                (g_pageRanges[j++] = info._addressRanges[i]._range)
                    .innerAlign(PAGE_SIZE_4K);
        }
        auto allocPhysBlock4K = [](usize amount) -> u64 {
            alignUpRef(amount, PAGE_SIZE_4K);
            Optional<u64> address = 0;
            for (usize i = 0; i < 256; i++) {
                if ((address = g_pageRanges[i].takeFront(amount)).IsPresent())
                    return address.Take();
            }
            return 0;
        };

        // Reserve space for page frame structures
        u64 const pages      = PAGE_SIZE_PARTITION / PAGE_SIZE_4K;
        u64       partitions = divCeil(info._limit, PAGE_SIZE_PARTITION);

        g_pageFrames = (Memory::PhysMemFrame**)allocPhysBlock4K(
            partitions * sizeof(Memory::PhysMemFrame**));
        for (int i = 0; i < partitions; i++) {
            g_pageFrames[i] = (Memory::PhysMemFrame*)allocPhysBlock4K(
                pages * sizeof(Memory::PhysMemFrame*));
        }

        // Load slub allocator
        for (int i = 0; i < SLAB_CACHE_BLOCK_AMOUNT; i++) {
            g_slabCaches[i] = new (
                (void*)::allocPhysMemory4K(sizeof(Memory::SlabCache)).unwrap())
                Memory::SlabCache(g_slabAmounts[i], 0);
        }

        return Ok();
    }
}

namespace Quark::System::Memory {

    Optional<PhysMemFrame*> PhysMemFrame::split()
    {
        if (_level == 0 || not _flags.has(Hal::PmmFlags::FREE)) {
            return Empty();
        }

        _level--;

        u64 offset = GetLevelAsOffset(_level, sizeof(PhysMemFrame));
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

    Optional<PhysMemFrame*> PhysMemFrame::merge(PhysMemFrame* page)
    {
        if (/* Ensure both pages are free and can be merged */
            !(_flags.has(Hal::PmmFlags::FREE) &&
              page->_flags.has(Hal::PmmFlags::FREE)) //
            ||
            /* Ensure these pages are connected in address */
            (abs(_address - page->_address) != (GetLevelAsOffset(_level)))) {
            return nullptr;
        }

        PhysMemFrame* result = (_address % (GetLevelAsOffset(_level) * 2)) //
                                   ? this
                                   : page;
        result->_level++;

        return result;
    }

    Optional<PhysMemFrame*> PhysMemFrame::merge()
    {
        u64 offset = GetLevelAsOffset(_level);
        if (!isPageAligned(_address, _level + 1)) {
            offset = -offset;
        }

        return merge((PhysMemFrame*)(this + offset));
    }

    PhysMemFrame* PhysMemFrame::at(u64 address)
    {
        return (PhysMemFrame*)(g_pageFrames + (address / PAGE_SIZE_4K));
    }
} // namespace Quark::System::Memory