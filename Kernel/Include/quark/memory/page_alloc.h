#pragma once

#include <mixins/meta/opt.h>
#include <mixins/meta/result.h>
#include <mixins/utils/array.h>
#include <mixins/utils/linked_queue.h>
#include <quark/memory/address_range.h>
#include <quark/memory/page.h>

#define BUDDY_LEVEL_UPPER_LIMIT 10
#define BUDDY_LEVEL_LOWER_LIMIT 0

namespace Quark::System::Memory {
    class PhysMemQueue
    {
    public:
        PhysMemQueue()
            : m_head(nullptr)
            , m_tail(nullptr)
            , m_count(0)
        {
        }

        PhysMemQueue(const PhysMemQueue& other)
        {
            if (!other.m_count) {
                return;
            }
            m_head  = other.m_head;
            m_tail  = other.m_tail;
            m_count = other.m_count;
        }

        PhysMemQueue(PhysMemQueue&& other)
        {
            if (!other.m_count) {
                return;
            }
            m_head  = other.m_head;
            m_tail  = other.m_tail;
            m_count = other.m_count;
        }

        void enqueue(PhysMemFrame* page);
        [[nodiscard]]
        Opt<PhysMemFrame*> dequeue();
        PhysMemFrame*      dequeue(PhysMemFrame* page);

        u32 count() const { return m_count; }

    private:
        PhysMemFrame *m_head, *m_tail;
        u32           m_count;
        u8            m_pageLevel;
    };

    extern Array<AddressRange[256]>                         g_pageRanges;
    extern Array<PhysMemQueue[BUDDY_LEVEL_UPPER_LIMIT + 1]> g_pageQueues;
    extern PhysMemFrame**                                   g_pageFrames;

} // namespace Quark::System::Memory