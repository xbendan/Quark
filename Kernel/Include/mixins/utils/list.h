#pragma once

#include <mixins/meta/opt.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/initializer_list.h>
#include <mixins/std/panic.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/iterator.h>

namespace Qk {

    template <typename TSource>
    class List
    {
        struct Node
        {
            TSource Data;
            Node*   Next;
            Node*   Prev;

            constexpr Node(TSource const& data,
                           Node*          next = nullptr,
                           Node*          prev = nullptr)
                : Data(Std::move(const_cast<TSource&>(data)))
                , Next(next)
                , Prev(prev)
            {
            }

            constexpr Node(TSource&& data,
                           Node*     next = nullptr,
                           Node*     prev = nullptr)
                : Data(Std::forward<TSource>(data))
                , Next(next)
                , Prev(prev)
            {
            }
        };

    public:
        // MARK: Constructors and Destructor

        List()
            : m_head(nullptr)
            , m_tail(nullptr)
            , m_size(0)
        {
        }

        List(List const& other)
            : m_head(nullptr)
            , m_tail(nullptr)
            , m_size(0)
        {
            Node* node = other.m_head;
            while (node) {
                PushBack(node->Data);
                node = node->Next;
            }
        }

        List(List&& other)
            : m_head(other.m_head)
            , m_tail(other.m_tail)
            , m_size(other.m_size)
        {
            other.m_head = nullptr;
            other.m_tail = nullptr;
            other.m_size = 0;
        }

        List(std::initializer_list<TSource> const& list)
            : m_head(nullptr)
            , m_tail(nullptr)
            , m_size(0)
        {
            for (auto& data : list) {
                PushBack(data);
            }
        }

        ~List()
        {
            while (m_head) {
                Node* node = m_head;
                m_head     = m_head->Next;
                delete node;
            }
        }

        // MARK: Operators

        List& operator+=(TSource const& data)
        {
            PushBack(data);
            return *this;
        }

        List& operator-=(TSource const& data)
        {
            Remove(data);
            return *this;
        }

        List& operator=(List const& other)
        {
            if (this == &other) {
                return *this;
            }

            RemoveAll();
            Node* node = other.m_head;
            while (node) {
                PushBack(node->Data);
                node = node->Next;
            }
            return *this;
        }

        List& operator=(List&& other)
        {
            if (this == &other) {
                return *this;
            }

            RemoveAll();
            m_head       = other.m_head;
            m_tail       = other.m_tail;
            m_size       = other.m_size;
            other.m_head = nullptr;
            other.m_tail = nullptr;
            other.m_size = 0;
            return *this;
        }

        // MARK: Indexer

        TSource& operator[](usize index)
        {
            if (index >= m_size) {
                Std::panic("Index out of bounds");
            }
            Node* node = m_head;
            while (index--) {
                node = node->Next;
            }
            return node->Data;
        }

        TSource const& operator[](usize index) const
        {
            if (index >= m_size) {
                Std::panic("Index out of bounds");
            }
            Node* node = m_head;
            while (index--) {
                node = node->Next;
            }
            return node->Data;
        }

        TSource& PushBack(TSource const& data)
        {
            auto* node = new Node(data);

            if (!m_head) {
                m_head = node;
                m_tail = node;
            } else {
                m_tail->Next = node;
                node->Prev   = m_tail;
                m_tail       = node;
            }
            m_size++;

            return node->Data;
        }

        TSource& PushBack(TSource&& data)
        {
            auto* node = new Node(data);

            if (!m_head) {
                m_head = node;
                m_tail = node;
            } else {
                m_tail->Next = node;
                node->Prev   = m_tail;
                m_tail       = node;
            }
            m_size++;

            return node->Data;
        }

        TSource& PushFront(TSource const& data)
        {
            auto* node = new Node(data);

            if (!m_head) {
                m_head = node;
                m_tail = node;
            } else {
                m_head->Prev = node;
                node->Next   = m_head;
                m_head       = node;
            }
            m_size++;

            return node->Data;
        }

        TSource& PushFront(TSource&& data)
        {
            auto* node = new Node(data);

            if (!m_head) {
                m_head = node;
                m_tail = node;
            } else {
                m_head->Prev = node;
                node->Next   = m_head;
                m_head       = node;
            }
            m_size++;

            return node->Data;
        }

        TSource PopBack()
        {
            if (!m_tail) {
                Std::panic("List is empty.");
            }

            TSource data(Std::move(m_tail->Data));
            Node*   node = m_tail;
            m_tail       = m_tail->Prev;
            if (m_tail) {
                m_tail->Next = nullptr;
            } else {
                m_head = nullptr;
            }
            delete node;
            m_size--;

            return data;
        }

        TSource PopFront()
        {
            if (!m_head) {
                Std::panic("List is empty.");
            }

            TSource data(Std::move(m_head->Data));
            Node*   node = m_head;
            m_head       = m_head->Next;
            if (m_head) {
                m_head->Prev = nullptr;
            } else {
                m_tail = nullptr;
            }
            delete node;
            m_size--;

            return data;
        }

        bool Remove(TSource const& data)
        {
            Node* node = m_head;
            while (node) {
                if (node->Data == data) {
                    if (node->Prev) {
                        node->Prev->Next = node->Next;
                    } else {
                        m_head = node->Next;
                    }
                    if (node->Next) {
                        node->Next->Prev = node->Prev;
                    } else {
                        m_tail = node->Prev;
                    }
                    delete node;
                    m_size--;
                    return true;
                }
                node = node->Next;
            }
            return false;
        }

        bool Remove(usize index)
        {
            if (index >= m_size) {
                return false;
            }
            Node* node = m_head;
            while (index--) {
                node = node->Next;
            }
            if (node->Prev) {
                node->Prev->Next = node->Next;
            } else {
                m_head = node->Next;
            }
            if (node->Next) {
                node->Next->Prev = node->Prev;
            } else {
                m_tail = node->Prev;
            }
            delete node;
            m_size--;
            return true;
        }

        TSource& Insert(TSource const& data, usize index)
        {
            return Insert(Std::move(data), index);
        }

        TSource& Insert(TSource&& data, usize index)
        {
            if (index >= m_size) {
                return PushBack(Std::forward<TSource>(data));
            }
            Node* node = m_head;
            while (index--) {
                node = node->Next;
            }
            Node* newNode = new Node(data, node, node->Prev);
            if (node->Prev) {
                node->Prev->Next = newNode;
            } else {
                m_head = newNode;
            }
            node->Prev = newNode;
            m_size++;
            return newNode->Data;
        }

        bool Contains(TSource const& data) const
        {
            Node* node = m_head;
            while (node) {
                if (node->Data == data) {
                    return true;
                }
                node = node->Next;
            }
            return false;
        }

        int IndexFirst(TSource const& data) const
        {
            Node* node  = m_head;
            int   index = 0;
            while (node) {
                if (node->Data == data) {
                    return index;
                }
                node = node->Next;
                index++;
            }
            return -1;
        }

        int IndexLast(TSource const& data) const
        {
            Node* node  = m_tail;
            int   index = m_size - 1;
            while (node) {
                if (node->Data == data) {
                    return index;
                }
                node = node->Prev;
                index--;
            }
            return -1;
        }

        void RemoveAll()
        {
            while (m_head) {
                Node* node = m_head;
                m_head     = m_head->Next;
                delete node;
            }
            m_head = nullptr;
            m_tail = nullptr;
            m_size = 0;
        }

        bool IsEmpty() const { return m_size == 0; }

        usize Count() const { return m_size; }

        // MARK: Language Integrated Query (LINQ) Functions

        TSource Sum()
            requires(Computable<TSource>)
        {
            TSource total = 0;
            Node*   node  = m_head;
            while (node) {
                total += node->Data;
                node = node->Next;
            }
            return total;
        }

        TSource Average()
            requires(Computable<TSource>)
        {
            return Sum() / m_size;
        }

        TSource Max()
            requires(Comparable<TSource>)
        {
            TSource max  = m_head->Data;
            Node*   node = m_head->Next;
            while (node) {
                if (node->Data > max) {
                    max = node->Data;
                }
                node = node->Next;
            }
            return max;
        }

        TSource Min()
            requires(Comparable<TSource>)
        {
            TSource min  = m_head->Data;
            Node*   node = m_head->Next;
            while (node) {
                if (node->Data < min) {
                    min = node->Data;
                }
                node = node->Next;
            }
            return min;
        }

        template <Computable TResult>
        TResult Sum(Func<TResult(TSource const&)> mapper)
        {
            TResult total = 0;
            Node*   node  = m_head;
            while (node) {
                total += mapper(node->Data);
                node = node->Next;
            }
            return total;
        }

        template <Computable TResult>
        TResult Average(Func<TResult(TSource const&)> mapper)
        {
            return Sum<TResult>(mapper) / m_size;
        }

        template <Comparable TResult>
        TResult Max(Func<TResult(TSource const&)> mapper)
        {
            TResult max  = mapper(m_head->Data);
            Node*   node = m_head->Next;
            while (node) {
                TResult result = mapper(node->Data);
                if (result > max) {
                    max = result;
                }
                node = node->Next;
            }
            return max;
        }

        template <Comparable TResult>
        TResult Min(Func<TResult(TSource const&)> mapper)
        {
            TResult min  = mapper(m_head->Data);
            Node*   node = m_head->Next;
            while (node) {
                TResult result = mapper(node->Data);
                if (result < min) {
                    min = result;
                }
                node = node->Next;
            }
            return min;
        }

        template <Comparable TKey>
        TSource& MaxBy(Func<TKey(TSource const&)> keySelector)
        {
            assert(m_size != 0, "List is empty.");

            TSource max    = m_head->Data;
            TKey    maxKey = keySelector(m_head->Data);
            Node*   node   = m_head->Next;
            while (node) {
                TKey key = keySelector(node->Data);
                if (key > maxKey) {
                    max    = node->Data;
                    maxKey = key;
                }
                node = node->Next;
            }
            return max;
        }

        template <Comparable TKey>
        TSource& MinBy(Func<TKey(TSource const&)> keySelector)
        {
            assert(m_size != 0, "List is empty.");

            TSource min    = m_head->Data;
            TKey    minKey = keySelector(m_head->Data);
            Node*   node   = m_head->Next;
            while (node) {
                TKey key = keySelector(node->Data);
                if (key < minKey) {
                    min    = node->Data;
                    minKey = key;
                }
                node = node->Next;
            }
            return min;
        }

        List<TSource>& Sort(bool ascending = true)
            requires(Comparable<TSource>)
        {
        }

        template <Comparable TKey>
        List<TSource>& SortBy(Func<TKey(TSource const&)> keySelector,
                              bool                       ascending)
        {
        }

        Opt<TSource&> FindFirst()
        {
            if (m_head) {
                return m_head->Data;
            }

            return Empty();
        }

        Opt<TSource&> FindFirst(Predicate<TSource const&> predicate)
        {
            Node* node = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    return node->Data;
                }
                node = node->Next;
            }
            return Empty();
        }

        Opt<TSource&> FindLast()
        {
            if (m_tail) {
                return m_tail->Data;
            }

            return Empty();
        }

        Opt<TSource&> FindLast(Predicate<TSource const&> predicate)
        {
            Node* node = m_tail;
            while (node) {
                if (predicate(node->Data)) {
                    return node->Data;
                }
                node = node->Prev;
            }
            return Empty();
        }

        Opt<TSource&> FindAny()
        {
            // TODO: Implement this function
        }

        List<TSource>* Take(usize n)
        {
            auto* newList = new List<TSource>();
            if (n > 0) {
                Node* node = m_head;
                while (n-- && node) {
                    newList->PushBack(node->Data);
                    node = node->Next;
                }
            }
            return newList;
        }

        List<TSource>* Take(Tuple<usize, usize> range)
        {
            auto* newList = new List<TSource>();
            if (range.v0 > 0) {
                Node* node = m_head;
                while (range.v0-- && node) {
                    node = node->Next;
                }
                while (range.v1-- && node) {
                    newList->PushBack(node->Data);
                    node = node->Next;
                }
            }
            return newList;
        }

        Opt<TSource> TakeFirst()
        {
            if (m_size == 0) {
                return Empty();
            }

            TSource data(Std::move(m_head->Data));
            Remove(m_head->Data);

            return data;
        }

        Opt<TSource> TakeFirst(Predicate<TSource const&> predicate)
        {
            Node* node = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    TSource data(Std::move(node->Data));
                    Remove(node->Data);
                    return data;
                }
                node = node->Next;
            }
            return Empty();
        }

        Opt<TSource> TakeLast()
        {
            if (m_size == 0) {
                return Empty();
            }

            TSource data(Std::move(m_tail->Data));
            Remove(m_tail->Data);

            return data;
        }

        Opt<TSource> TakeLast(Predicate<TSource const&> predicate)
        {
            Node* node = m_tail;
            while (node) {
                if (predicate(node->Data)) {
                    TSource data(Std::move(node->Data));
                    Remove(node->Data);
                    return data;
                }
                node = node->Prev;
            }
            return Empty();
        }

        Opt<TSource> TakeAt(usize index)
        {
            if (index >= m_size) {
                return Empty();
            }

            Node* node = m_head;
            while (index--) {
                node = node->Next;
            }

            TSource data(Std::move(node->Data));
            Remove(node->Data);

            return data;
        }

        List<TSource>* TakeWhile(Predicate<TSource const&> predicate)
        {
            auto* newList = new List<TSource>();
            Node* node    = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    newList->PushBack(node->Data);
                } else {
                    break;
                }
                node = node->Next;
            }
            return newList;
        }

        TSource& Single()
        {
            assert(m_size == 1, "Sequence contains more than one element");
            return m_head->Data;
        }

        TSource& Single(Predicate<TSource const&> predicate)
        {
            usize index = -1;
            Node* node  = m_head;
            for (usize i = 0; i < m_size; i++) {
                if (predicate(node->Data)) {
                    if (index != -1) {
                        assert(false,
                               "Sequence contains more than one element");
                    }
                    index = i;
                }
                node = node->Next;
            }
            assert(index != -1, "Sequence contains no element");
            return m_head->Data;
        }

        Opt<TSource&> SingleOrDefault(TSource const& defaultValue)
        {
            assert(m_size <= 1, "Sequence contains more than one element");
            return m_size ? m_head->Data : defaultValue;
        }

        TSource& DefaultIfEmpty(TSource const& defaultValue)
        {
            return m_size ? m_head->Data : defaultValue;
        }

        List<TSource>* Skip(usize n)
        {
            auto* newList = new List<TSource>();
            Node* node    = m_head;
            while (n-- && node) {
                node = node->Next;
            }
            while (node) {
                newList->PushBack(node->Data);
                node = node->Next;
            }
            return newList;
        }

        List<TSource>* SkipLast(usize n)
        {
            auto* newList = new List<TSource>();
            Node* node    = m_tail;
            while (n-- && node) {
                node = node->Prev;
            }
            while (node) {
                newList->PushBack(node->Data);
                node = node->Prev;
            }
            return newList;
        }

        List<TSource>* SkipWhile(Predicate<TSource const&> predicate)
        {
            auto* newList = new List<TSource>();
            Node* node    = m_head;
            while (node) {
                if (!predicate(node->Data)) {
                    newList->PushBack(node->Data);
                }
                node = node->Next;
            }
            return newList;
        }

        void ForEach(Func<void(TSource&)> action)
        {
            Node* node = m_head;
            while (node) {
                action(node->Data);
                node = node->Next;
            }
        }

        void ForEachOrdered(Func<void(TSource&, usize)> action)
        {
            Node* node = m_head;
            usize i    = 0;
            while (node) {
                action(node->Data, i++);
                node = node->Next;
            }
        }

        template <typename TResult>
        List<TResult>* Select(Func<TResult(TSource const&)> selector)
        {
            auto* newList = new List<TResult>();
            Node* node    = m_head;
            while (node) {
                newList->PushBack(selector(node->Data));
                node = node->Next;
            }
            return newList;
        }

        bool AllMatch(Predicate<TSource const&> predicate) const
        {
            Node* node = m_head;
            while (node) {
                if (!predicate(node->Data)) {
                    return false;
                }
                node = node->Next;
            }
            return true;
        }

        bool AnyMatch(Predicate<TSource const&> predicate) const
        {
            Node* node = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    return true;
                }
                node = node->Next;
            }
            return false;
        }

        bool NoneMatch(Predicate<TSource const&> predicate) const
        {
            Node* node = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    return false;
                }
                node = node->Next;
            }
            return true;
        }

        List<TSource>* Where(Predicate<TSource const&> predicate)
        {
            auto* newList = new List<TSource>();
            Node* node    = m_head;
            while (node) {
                if (predicate(node->Data)) {
                    newList->PushBack(node->Data);
                }
                node = node->Next;
            }
            return newList;
        }

    private:
        Node* m_head;
        Node* m_tail;
        usize m_size;
    };
}

// template <typename TSource>
// struct LinkedNode
// {
//     TSource* _prev;
//     TSource* _next;
// };

// template <typename TSource>
// class LinkedList : public IList<TSource>
// {
//     using TSourceReference =
//         Std::Conditional<Std::isScalar<TSource>, TSource, TSource const&>;

//     struct Node
//     {
//         TSource _data;
//         Node*   _next;
//         Node*   _prev;

//         constexpr Node(TSource const& data,
//                        Node*          next = nullptr,
//                        Node*          prev = nullptr)
//             : _data(Std::move(const_cast<TSource&>(data)))
//             , _next(next)
//             , _prev(prev)
//         {
//         }
//     };

//     // struct _Iter : public IIterator<TSource>
//     // {
//     //     _Iter(Node* node)
//     //         : _node(node)
//     //     {
//     //     }

//     //     TSource& current() const override { return _node->_data; }

//     //     TSource& next() override
//     //     {
//     //         assert(_node->_next, "Iterator out of bounds.");

//     //         _node = _node->_next;
//     //         return _node->_data;
//     //     }

//     //     bool hasNext() const override { return _node != nullptr; }

//     // private:
//     //     Node* _node;
//     // };

// public:
//     LinkedList() = default;
//     LinkedList(LinkedList const& other)
//         : _head(nullptr)
//         , _tail(nullptr)
//         , _size(0)
//     {
//         Node* node = other._head;
//         while (node) {
//             Add(node->_data);
//             node = node->_next;
//         }
//     }
//     LinkedList(LinkedList&& other)
//         : _head(other._head)
//         , _tail(other._tail)
//         , _size(other._size)
//     {
//         other._head = nullptr;
//         other._tail = nullptr;
//         other._size = 0;
//     }
//     LinkedList(ICollection<TSource> const& collection)
//         : _head(nullptr)
//         , _tail(nullptr)
//         , _size(0)
//     {
//         collection.ForEach([&](TSource const& data) { Add(data); });
//     }
//     LinkedList(std::initializer_list<TSource> const& list)
//         : _head(nullptr)
//         , _tail(nullptr)
//         , _size(0)
//     {
//         for (auto& data : list) {
//             Add(data);
//         }
//     }
//     ~LinkedList()
//     {
//         while (_head) {
//             Node* node = _head;
//             _head      = _head->_next;
//             delete node;
//         }
//     }

//     LinkedList& operator=(LinkedList const& other)
//     {
//         if (this == &other) {
//             return *this;
//         }
//         Clear();
//         Node* node = other._head;
//         while (node) {
//             Add(node->_data);
//             node = node->_next;
//         }
//         return *this;
//     }

//     LinkedList& operator=(LinkedList&& other)
//     {
//         if (this == &other) {
//             return *this;
//         }
//         Clear();
//         _head       = other._head;
//         _tail       = other._tail;
//         _size       = other._size;
//         other._head = nullptr;
//         other._tail = nullptr;
//         other._size = 0;
//         return *this;
//     }

//     LinkedList& operator=(ICollection<TSource> const& collection)
//     {
//         Clear();
//         collection.ForEach([&](TSource const& data) { Add(data); });
//         return *this;
//     }

//     bool operator==(LinkedList const& other) const
//     {
//         if (_size != other._size) {
//             return false;
//         }
//         Node* node      = _head;
//         Node* otherNode = other._head;
//         while (node) {
//             if (node->_data != otherNode->_data) {
//                 return false;
//             }
//             node      = node->_next;
//             otherNode = otherNode->_next;
//         }
//         return true;
//     }

//     bool operator!=(LinkedList const& other) const { return !(*this ==
//     other); }

//     TSource& Add(TSource const& data) override
//     {
//         Node* node = new Node(data);
//         if (!_head) {
//             _head = node;
//             _tail = node;
//         } else {
//             _tail->_next = node;
//             node->_prev  = _tail;
//             _tail        = node;
//         }
//         _size++;
//         return node->_data;
//     }

//     bool Remove(TSource const& data) override
//     {
//         Node* node = _head;
//         while (node) {
//             if (node->_data == data) {
//                 if (node->_prev) {
//                     node->_prev->_next = node->_next;
//                 } else {
//                     _head = node->_next;
//                 }
//                 if (node->_next) {
//                     node->_next->_prev = node->_prev;
//                 } else {
//                     _tail = node->_prev;
//                 }
//                 delete node;
//                 _size--;
//                 return true;
//             }
//             node = node->_next;
//         }
//         return false;
//     }

//     bool RemoveAt(usize index) override
//     {
//         if (index >= _size) {
//             return false;
//         }
//         Node* node = _head;
//         while (index--) {
//             node = node->_next;
//         }
//         if (node->_prev) {
//             node->_prev->_next = node->_next;
//         } else {
//             _head = node->_next;
//         }
//         if (node->_next) {
//             node->_next->_prev = node->_prev;
//         } else {
//             _tail = node->_prev;
//         }
//         delete node;
//         _size--;
//         return true;
//     }

//     TSource& pushBack(TSource const& data) { return Add(data); }

//     TSource& pushFront(TSource const& data)
//     {
//         Node* node = new Node(Std::move(data), _head, nullptr);
//         if (_head) {
//             _head->_prev = node;
//         } else {
//             _tail = node;
//         }
//         _head = node;
//         _size++;
//         return node->_data;
//     }

//     TSource popBack()
//     {
//         if (!_tail) {
//             return nullptr;
//         }
//         Node* node = _tail;
//         if (_tail->prev) {
//             _tail->prev->next = nullptr;
//             _tail             = _tail->prev;
//         } else {
//             _head = nullptr;
//             _tail = nullptr;
//         }
//         TSource data(Std::move(node->_data));
//         delete node;
//         _size--;
//         return data;
//     }

//     TSource popFront()
//     {
//         if (!_head) {
//             return nullptr;
//         }
//         Node* node = _head;
//         if (_head->next) {
//             _head->next->prev = nullptr;
//             _head             = _head->next;
//         } else {
//             _head = nullptr;
//             _tail = nullptr;
//         }
//         TSource data(Std::move(node->_data));
//         delete node;
//         _size--;
//         return data;
//     }

//     bool Contains(TSource const& data) const override
//     {
//         Node* node = _head;
//         while (node) {
//             if (node->_data == data) {
//                 return true;
//             }
//             node = node->_next;
//         }
//         return false;
//     }

//     usize Count() const override { return _size; }

//     TSource& Insert(TSource const& data, usize index) override
//     {
//         if (index >= _size) {
//             return Add(data);
//         }
//         Node* node = _head;
//         while (index--) {
//             node = node->_next;
//         }
//         Node* newNode = new Node(Std::move(data), node, node->_prev);
//         if (node->_prev) {
//             node->_prev->_next = newNode;
//         } else {
//             _head = newNode;
//         }
//         node->_prev = newNode;
//         _size++;
//         return newNode->_data;
//     }

//     // TSource& get(usize index) override
//     // {
//     //     if (index >= _size) {
//     //         return nullptr;
//     //     }
//     //     Node* node = _head;
//     //     while (index--) {
//     //         node = node->_next;
//     //     }
//     //     return node->_data;
//     // }

//     int IndexOf(TSource const& data) const override
//     {
//         Node* node  = _head;
//         int   index = 0;
//         while (node) {
//             if (node->_data == data) {
//                 return index;
//             }
//             node = node->_next;
//             index++;
//         }
//         return -1;
//     }

//     int LastIndexOf(TSource const& data) const override
//     {
//         Node* node  = _tail;
//         int   index = _size - 1;
//         while (node) {
//             if (node->_data == data) {
//                 return index;
//             }
//             node = node->_prev;
//             index--;
//         }
//         return -1;
//     }

//     bool IsEmpty() const override { return _size == 0; }

//     void Clear() override
//     {
//         while (_head) {
//             Node* node = _head;
//             _head      = _head->_next;
//             delete node;
//         }
//         _head = nullptr;
//         _tail = nullptr;
//         _size = 0;
//     }

//     // Linq Functions

//     TSource Sum()
//         requires(Computable<TSource>)
//     {
//         TSource total = 0;
//         Node*   node  = _head;
//         while (node) {
//             total += node->_data;
//             node = node->_next;
//         }
//         return total;
//     }

//     TSource Average()
//         requires(Computable<TSource>)
//     {
//         return Sum() / Count();
//     }

//     TSource Max()
//         requires(Computable<TSource>)
//     {
//         TSource max  = _head->data;
//         Node*   node = _head->next;
//         while (node) {
//             if (node->_data > max) {
//                 max = node->_data;
//             }
//             node = node->_next;
//         }
//         return max;
//     }

//     TSource Min()
//         requires(Computable<TSource>)
//     {
//         TSource min  = _head->data;
//         Node*   node = _head->next;
//         while (node) {
//             if (node->_data < min) {
//                 min = node->_data;
//             }
//             node = node->_next;
//         }
//         return min;
//     }

//     template <Computable TResult>
//     TResult Sum(Func<TResult(TSource const&)> mapper)
//     {
//         TResult total = 0;
//         Node*   node  = _head;
//         while (node) {
//             total += mapper(node->_data);
//             node = node->_next;
//         }
//         return total;
//     }

//     template <Computable TResult>
//     TResult Average(Func<TResult(TSource const&)> mapper)
//     {
//         return Sum<TResult>(mapper) / Count();
//     }

//     template <Comparable TResult>
//     TResult Max(Func<TResult(TSource const&)> mapper)
//     {
//         TResult max  = mapper(_head->data);
//         Node*   node = _head->next;
//         while (node) {
//             TResult result = mapper(node->_data);
//             if (result > max) {
//                 max = result;
//             }
//             node = node->_next;
//         }
//         return max;
//     }

//     template <Comparable TResult>
//     TResult Min(Func<TResult(TSource const&)> mapper)
//     {
//         TResult min  = mapper(_head->data);
//         Node*   node = _head->next;
//         while (node) {
//             TResult result = mapper(node->_data);
//             if (result < min) {
//                 min = result;
//             }
//             node = node->_next;
//         }
//         return min;
//     }

//     template <Comparable TKey>
//     TSource MaxBy(Func<TKey(TSource const&)> selector)
//     {
//         TSource& max  = _head->data;
//         TKey     key  = selector(_head->data);
//         Node*    node = _head->next;
//         while (node) {
//             TKey result = selector(node->_data);
//             if (result > key) {
//                 key = result;
//                 max = node->_data;
//             }
//             node = node->_next;
//         }
//         return max;
//     }
//     template <Comparable TKey>
//     TSource MinBy(Func<TKey(TSource const&)> selector)
//     {
//         TSource& min  = _head->data;
//         TKey     key  = selector(_head->data);
//         Node*    node = _head->next;
//         while (node) {
//             TKey result = selector(node->_data);
//             if (result < key) {
//                 key = result;
//                 min = node->_data;
//             }
//             node = node->_next;
//         }
//         return min;
//     }

//     IList<TSource>& Order(bool ascending = true)
//         requires(Comparable<TSource>)
//     {
//     }

//     template <Comparable TKey>
//     IList<TSource>& OrderBy(Func<TKey(TSource const&)> keySelector,
//                             bool                       ascending)
//     {
//     }

//     Opt<TSource&> FindFirst() override
//     {
//         if (_head) {
//             return _head->_data;
//         }

//         return Empty();
//     }

//     Opt<TSource&> FindFirst(Predicate<TSource const&> predicate) override
//     {
//         Node* node = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 return &node->_data;
//             }
//             node = node->_next;
//         }
//         return Empty();
//     }

//     Opt<TSource&> FindLast() override { return _tail->_data; }

//     Opt<TSource&> FindLast(Predicate<TSource const&> predicate) override
//     {
//         Node* node = _tail;
//         while (node) {
//             if (predicate(node->_data)) {
//                 return &node->_data;
//             }
//             node = node->_prev;
//         }
//         return Empty();
//     }

//     Opt<TSource&> FindAny() override { return _head->_data; }

//     Opt<TSource&> FindAny(Predicate<TSource const&> predicate) override
//     {
//         Node* node = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 return &node->_data;
//             }
//             node = node->_next;
//         }
//         return Empty();
//     }

//     IList<TSource>& Take(usize n) override
//     {
//         auto newList = new LinkedList<TSource>();
//         if (n > 0) {
//             Node* node = _head;
//             while (n-- && node) {
//                 newList->Add(node->_data);
//                 node = node->_next;
//             }
//         }
//         return *newList;
//     }

//     IList<TSource>& Take(Tuple<usize, usize> range) override
//     {
//         auto newList = new LinkedList<TSource>();
//         if (range.v0 > 0) {
//             Node* node = _head;
//             while (range.v0-- && node) {
//                 node = node->_next;
//             }
//             while (range.v1-- && node) {
//                 newList->Add(node->_data);
//                 node = node->_next;
//             }
//         }
//         return *newList;
//     }

//     Opt<TSource> TakeFirst()
//     {
//         if (_size == 0) {
//             return Empty();
//         }

//         TSource data(Std::move(_head->_data));
//         RemoveAt(0);

//         return data;
//     }

//     Opt<TSource> TakeFirst(Predicate<TSource const&> predicate)
//     {
//         if (_size == 0) {
//             Std::panic("Sequence contains no elements");
//         }

//         Node* node = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 TSource data(Std::move(node->_data));

//                 // FIXME: 在使用移动语义后原始数据会被清空
//                 //        在删除节点时可能导致删除错误节点
//                 Remove(node->_data);
//                 return data;
//             }
//             node = node->_next;
//         }

//         return Empty();
//     }

//     IList<TSource>& TakeLast(usize n) override
//     {
//         auto newList = new LinkedList<TSource>();
//         if (n > 0) {
//             Node* node = _tail;
//             while (n-- && node) {
//                 newList->pushFront(node->_data);
//                 node = node->_prev;
//             }
//         }
//         return *newList;
//     }

//     Opt<TSource> TakeLast()
//     {
//         if (_size == 0) {
//             return Empty();
//         }

//         TSource data(Std::move(_tail->_data));
//         RemoveAt(_size - 1);

//         return data;
//     }

//     IList<TSource>& TakeWhile(Func<bool(TSource const&)> predicate) override
//     {
//         auto  newList = new LinkedList<TSource>();
//         Node* node    = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 newList->Add(node->_data);
//             }
//             node = node->_next;
//         }
//         return *newList;
//     }

//     TSource& Single() override
//     {
//         if (_size != 1) {
//             Std::panic("Sequence contains more than one element");
//         }
//         return _head->_data;
//     }

//     TSource& Single(Predicate<TSource const&> predicate) {}

//     TSource& SingleOrDefault(TSource const& defaultValue) override
//     {
//         if (_size > 1) {
//             Std::panic("Sequence contains more than one element");
//         }
//         if (_size == 0) {
//             return const_cast<TSource&>(defaultValue);
//         }
//         return _head->_data;
//     }

//     TSource& DefaultIfEmpty()
//         requires(Constructible<TSource>)
//     {
//         if (_size == 0) {
//             return TSource();
//         }
//         return _head->data;
//     }

//     TSource& DefaultIfEmpty(TSource const& defaultValue) override
//     {
//         if (_size == 0) {
//             return const_cast<TSource&>(defaultValue);
//         }
//         return Count() ? _head->_data : const_cast<TSource&>(defaultValue);
//     }

//     IList<TSource>& Skip(usize n) override
//     {
//         auto newList = new LinkedList<TSource>();
//         if (n < _size) {
//             Node* node = _head;
//             while (n-- && node) {
//                 node = node->_next;
//             }
//             while (node) {
//                 newList->Add(node->_data);
//                 node = node->_next;
//             }
//         }
//         return *newList;
//     }

//     IList<TSource>& SkipLast(usize n) override
//     {
//         auto newList = new LinkedList<TSource>();
//         if (n < _size) {
//             Node* node = _tail;
//             while (n-- && node) {
//                 node = node->_prev;
//             }
//             while (node) {
//                 newList->pushFront(node->_data);
//                 node = node->_prev;
//             }
//         }
//         return *newList;
//     }

//     IList<TSource>& SkipWhile(Func<bool(TSource const&)> predicate) override
//     {
//         auto* newList = new LinkedList<TSource>();
//         Node* node    = _head;
//         while (node) {
//             if (!predicate(node->_data)) {
//                 newList->Add(node->_data);
//             }
//             node = node->_next;
//         }
//         return *newList;
//     }

//     // template <class TCollection, typename TKey>
//     // TCollection<T>& groupBy(Func<TKey(TSource const&)> keySelector)
//     // {
//     // }

//     void ForEach(Func<void(TSource const&)> action) const override
//     {
//         Node* node = _head;
//         while (node) {
//             action(node->_data);
//             node = node->_next;
//         }
//     }

//     void ForEachOrdered(Func<void(TSource const&, usize)> action) const
//     override
//     {
//         Node* node = _head;
//         usize i    = 0;
//         while (node) {
//             action(node->_data, i++);
//             node = node->_next;
//         }
//     }

//     template <typename TResult>
//     IList<TResult>* Select(Func<TResult(TSource const&)> selector)
//     {
//         auto* newList = new ArrayList<TResult>(this->Count());
//         Node* node    = _head;
//         while (node) {
//             newList->Add(selector(node->_data));
//             node = node->_next;
//         }
//         return newList;
//     }

//     bool AllMatch(Func<bool(TSource const&)> predicate) const override
//     {
//         Node* node = _head;
//         while (node) {
//             if (!predicate(node->_data)) {
//                 return false;
//             }
//             node = node->_next;
//         }
//         return true;
//     }

//     bool AnyMatch(Func<bool(TSource const&)> predicate) const override
//     {
//         Node* node = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 return true;
//             }
//             node = node->_next;
//         }
//         return false;
//     }

//     bool NoneMatch(Func<bool(TSource const&)> predicate) const override
//     {
//         Node* node = _head;
//         while (node) {
//             if (predicate(node->_data)) {
//                 return false;
//             }
//             node = node->_next;
//         }
//         return true;
//     }

//     LinkedList<TSource>& operator+=(TSource const& data) override
//     {
//         Add(data);
//         return *this;
//     }

//     LinkedList<TSource>& operator-=(TSource const& data) override
//     {
//         Remove(data);
//         return *this;
//     }

//     TSource& operator[](usize const& index) override
//     {
//         if (index >= _size)
//             Std::panic(Error::IndexOutOfBounds("Index out of bounds"));

//         usize i    = index;
//         Node* node = _head;
//         while (i--) {
//             node = node->_next;
//         }
//         return node->_data;
//     }

//     TSource const& operator[](usize const& index) const override
//     {
//         if (index >= _size)
//             Std::panic(Error::IndexOutOfBounds("Index out of bounds"));

//         usize i    = index;
//         Node* node = _head;
//         while (i--) {
//             node = node->_next;
//         }
//         return node->_data;
//     }

// private:
//     Node* _head = nullptr;
//     Node* _tail = nullptr;
//     usize _size = 0;
// };