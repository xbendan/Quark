#include <quark/memory/page.h>

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
        Optional<PhysMemFrame*> dequeue();
        PhysMemFrame*           dequeue(PhysMemFrame* page);

        u32 count() const { return m_count; }

    private:
        PhysMemFrame *m_head, *m_tail;
        u32           m_count;
        u8            m_pageLevel;
    };
}