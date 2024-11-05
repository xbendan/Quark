#pragma once

#include <mixins/utils/collections.h>
#include <mixins/utils/list.h>

namespace Qk {
    template <typename TSource>
    struct OneWayNode
    {
        OneWayNode* Next;
        TSource     Value;
    };

    template <typename TSource>
    class Queue
    {
    public:
        Queue() = default;

        TSource& PushBack(TSource const& e)
        {
            auto* node  = new OneWayNode<TSource>{};
            node->Value = e;
            if (m_head == nullptr) {
                m_head = node;
            } else {
                m_last->Next = node;
            }
            m_last = node;
            m_size++;
            return node->Value;
        }

        Opt<TSource> PopFront()
        {
            if (m_head == nullptr) {
                return {};
            }
            auto* node    = m_head;
            m_head        = m_head->Next;
            TSource value = node->Value;
            delete node;
            m_size--;
            return value;
        }

        Opt<TSource&> Peek()
        {
            if (m_head == nullptr) {
                return Empty();
            }
            return m_head->Value;
        }

        bool Remove(TSource const& e)
        {
            OneWayNode<TSource>* prev = nullptr;
            OneWayNode<TSource>* node = m_head;
            while (node) {
                if (node->Value == e) {
                    if (prev) {
                        prev->Next = node->Next;
                    } else {
                        m_head = node->Next;
                    }
                    delete node;
                    m_size--;
                    return true;
                }
                prev = node;
                node = node->Next;
            }
            return false;
        }

        usize RemoveAll()
        {
            auto* node = m_head;
            while (node) {
                auto* next = node->Next;
                delete node;
                node = next;
            }
            m_head = nullptr;

            usize len = m_size;
            m_size    = 0;
            return len;
        }

        bool Contains(TSource const& e) const
        {
            OneWayNode<TSource>* node = m_head;
            while (node) {
                if (node->Value == e) {
                    return true;
                }
                node = node->Next;
            }
            return false;
        }

        bool IsEmpty() const { return m_size == 0; }

        usize Count() const { return m_size; }

    private:
        OneWayNode<TSource>* m_head = nullptr;
        OneWayNode<TSource>* m_last = nullptr;
        usize                m_size = 0;
    };
}