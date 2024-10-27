#include <quark/memory/page_queue.h>

namespace Quark::System::Memory {
    void PageQueue::Enqueue(PageFrame* page)
    {
        if (!m_count) {
            m_head = page;
            m_tail = page;
        } else {
            m_tail->_next   = page;
            page->_previous = m_tail;
            m_tail          = page;
        }
        m_count++;
    }

    PageFrame* PageQueue::Dequeue()
    {
        if (!m_count) {
            return nullptr;
        }
        PageFrame* page = m_head;
        m_head          = m_head->_next;
        if (m_head) {
            m_head->_previous = nullptr;
        } else {
            m_tail = nullptr;
        }
        m_count--;
        return page;
    }

    PageFrame* PageQueue::Dequeue(PageFrame* page)
    {
        if (page == nullptr) {
            return nullptr;
        }

        if (page->_previous) {
            page->_previous->_next = page->_next;
        } else {
            m_head = page->_next;
        }

        if (page->_next) {
            page->_next->_previous = page->_previous;
        } else {
            m_tail = page->_previous;
        }

        m_count--;
        return page;
    }
} // namespace Quark::System::Memory