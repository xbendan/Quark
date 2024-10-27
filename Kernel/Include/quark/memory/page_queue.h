#pragma once

#include <quark/memory/page.h>

namespace Quark::System::Memory {
    class PageQueue
    {
    public:
        void       Enqueue(PageFrame* page);
        PageFrame* Dequeue();
        PageFrame* Dequeue(PageFrame* page);

        u32 Count() const { return m_count; }

    private:
        PageFrame *m_head, *m_tail;
        u32        m_count;
    };
}