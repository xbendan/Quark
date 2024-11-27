#include <quark/init/boot_info.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>
#include <quark/memory/slab.h>
#include <quark/os/main.h>

#include <mixins/math/compute.h>

namespace Quark::System::Memory {

    Optional<PageFrame*> PageFrame::Divide()
    {
        if (_level == 0 || not _flags.has(Hal::PmmFlags::FREE)) {
            return Empty();
        }

        _level--;

        // u64 offset = GetLevelAsOffset(_level, sizeof(PhysMemFrame));

        return new (this + (1 << _level)) PageFrame{
            ._flags   = _flags,
            ._level   = _level,
            ._chainLength = (u16)(1 << _level),
            ._kmem = {
                ._inuse   = 0,
                ._objects = 0,
                ._frozen  = 0,
            },
            ._pageHead = _pageHead,
            ._address  = _address + GetLevelAsOffset(_level, PAGE_SIZE_4K),
        };
    }

    Optional<PageFrame*> PageFrame::Combine(PageFrame* page)
    {
        if (/* Ensure both pages are free and can be merged */
            !(_flags.has(Hal::PmmFlags::FREE) &&
              page->_flags.has(Hal::PmmFlags::FREE)) //
            ||
            /* Ensure these pages are connected in address */
            (abs(_address - page->_address) != (GetLevelAsOffset(_level)))) {
            return nullptr;
        }

        PageFrame* result = (_address % (GetLevelAsOffset(_level) * 2)) //
                                ? this
                                : page;
        result->_level++;

        return result;
    }

    Optional<PageFrame*> PageFrame::Combine()
    {
        u64 offset = GetLevelAsOffset(_level);
        if (!IsPageAligned(_address, _level + 1)) {
            offset = -offset;
        }

        return Combine((PageFrame*)(this + offset));
    }

    PageFrame* PageFrame::ByAddress(u64 address)
    {
        u32 par = address / PAGE_SIZE_PARTITION;
        u32 idx =
            (address / PAGE_SIZE_4K) % (PAGE_SIZE_PARTITION / PAGE_SIZE_4K);
        return g_pageFrames[par] + idx;

        // return (PhysMemFrame*)(g_pageFrames + (address / PAGE_SIZE_4K));
    }
} // namespace Quark::System::Memory