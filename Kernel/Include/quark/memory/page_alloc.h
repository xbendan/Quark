#pragma once

#include <mixins/meta/opt.h>
#include <mixins/meta/result.h>
#include <mixins/utils/array.h>
#include <mixins/utils/linked_queue.h>
#include <quark/memory/address_range.h>
#include <quark/memory/page.h>
#include <quark/memory/page_queue.h>

#define BUDDY_LEVEL_UPPER_LIMIT 10
#define BUDDY_LEVEL_LOWER_LIMIT 0

namespace Quark::System::Memory {
    extern Array<AddressRange[256]>                         g_pageRanges;
    extern Array<PhysMemQueue[BUDDY_LEVEL_UPPER_LIMIT + 1]> g_pageQueues;
    extern PhysMemFrame**                                   g_pageFrames;

} // namespace Quark::System::Memory