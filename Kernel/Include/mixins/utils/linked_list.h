#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/panic.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/iterator.h>

template <typename TSource>
class LinkedList
    : public IList<TSource>
    , public IReadOnlyList<TSource>
{
    struct Node
    {
        TSource _data;
        Node*   _next;
        Node*   _prev;

        constexpr Node(TSource const& data)
            : _data(Std::move(data))
        {
        }

        constexpr Node(TSource const& data, Node* next, Node* prev)
            : _data(Std::move(data))
            , _next(next)
            , _prev(prev)
        {
        }
    };

public:
    LinkedList() = default;
    LinkedList(LinkedList const& other)
        : _head(nullptr)
        , _tail(nullptr)
        , _size(0)
    {
        Node* node = other._head;
        while (node) {
            add(node->_data);
            node = node->_next;
        }
    }
    LinkedList(LinkedList&& other)
        : _head(other._head)
        , _tail(other._tail)
        , _size(other._size)
    {
        other._head = nullptr;
        other._tail = nullptr;
        other._size = 0;
    }
    ~LinkedList()
    {
        while (_head) {
            Node* node = _head;
            _head      = _head->_next;
            delete node;
        }
    }

    LinkedList& operator=(LinkedList const& other)
    {
        if (this == &other) {
            return *this;
        }
        clear();
        Node* node = other._head;
        while (node) {
            pushBack(node->_data);
            node = node->_next;
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other)
    {
        if (this == &other) {
            return *this;
        }
        clear();
        _head       = other._head;
        _tail       = other._tail;
        _size       = other._size;
        other._head = nullptr;
        other._tail = nullptr;
        other._size = 0;
        return *this;
    }

    TSource& add(TSource const& data) override
    {
        Node* node = new Node(data);
        if (!_head) {
            _head = node;
            _tail = node;
        } else {
            _tail->_next = node;
            node->_prev  = _tail;
            _tail        = node;
        }
        _size++;
        return node->_data;
    }

    bool remove(TSource const& data) override
    {
        Node* node = _head;
        while (node) {
            if (node->_data == data) {
                if (node->_prev) {
                    node->_prev->_next = node->_next;
                } else {
                    _head = node->_next;
                }
                if (node->_next) {
                    node->_next->_prev = node->_prev;
                } else {
                    _tail = node->_prev;
                }
                delete node;
                _size--;
                return true;
            }
            node = node->_next;
        }
        return false;
    }

    bool removeAt(usize index) override
    {
        if (index >= _size) {
            return false;
        }
        Node* node = _head;
        while (index--) {
            node = node->_next;
        }
        if (node->_prev) {
            node->_prev->_next = node->_next;
        } else {
            _head = node->_next;
        }
        if (node->_next) {
            node->_next->_prev = node->_prev;
        } else {
            _tail = node->_prev;
        }
        delete node;
        _size--;
        return true;
    }

    TSource& pushBack(TSource const& data) { return add(data); }

    TSource& pushFront(TSource const& data)
    {
        Node* node = new Node(data, _head, nullptr);
        if (_head) {
            _head->prev = node;
        } else {
            _tail = node;
        }
        _head = node;
        _size++;
        return node->_data;
    }

    TSource popBack()
    {
        if (!_tail) {
            return nullptr;
        }
        Node* node = _tail;
        if (_tail->prev) {
            _tail->prev->next = nullptr;
            _tail             = _tail->prev;
        } else {
            _head = nullptr;
            _tail = nullptr;
        }
        TSource data(Std::move(node->_data));
        delete node;
        _size--;
        return data;
    }

    TSource popFront()
    {
        if (!_head) {
            return nullptr;
        }
        Node* node = _head;
        if (_head->next) {
            _head->next->prev = nullptr;
            _head             = _head->next;
        } else {
            _head = nullptr;
            _tail = nullptr;
        }
        TSource data = Std::move(node->_data);
        delete node;
        _size--;
        return data;
    }

    bool contains(TSource const& data) override
    {
        Node* node = _head;
        while (node) {
            if (node->_data == data) {
                return true;
            }
            node = node->_next;
        }
        return false;
    }

    bool contains(TSource const& data) const override
    {
        Node* node = _head;
        while (node) {
            if (node->_data == data) {
                return true;
            }
            node = node->_next;
        }
        return false;
    }

    usize count() const override { return _size; }

    TSource& insert(TSource const& data, usize index) override
    {
        if (index >= _size) {
            return add(data);
        }
        Node* node = _head;
        while (index--) {
            node = node->_next;
        }
        Node* newNode = new Node(data, node, node->_prev);
        if (node->_prev) {
            node->_prev->_next = newNode;
        } else {
            _head = newNode;
        }
        node->_prev = newNode;
        _size++;
        return newNode->_data;
    }

    // TSource& get(usize index) override
    // {
    //     if (index >= _size) {
    //         return nullptr;
    //     }
    //     Node* node = _head;
    //     while (index--) {
    //         node = node->_next;
    //     }
    //     return node->_data;
    // }

    int indexOf(TSource const& data) const override
    {
        Node* node  = _head;
        int   index = 0;
        while (node) {
            if (node->_data == data) {
                return index;
            }
            node = node->_next;
            index++;
        }
        return -1;
    }

    int lastIndexOf(TSource const& data) const override
    {
        Node* node  = _tail;
        int   index = _size - 1;
        while (node) {
            if (node->_data == data) {
                return index;
            }
            node = node->_prev;
            index--;
        }
        return -1;
    }

    bool isEmpty() const override { return _size == 0; }

    void clear() override
    {
        while (_head) {
            Node* node = _head;
            _head      = _head->_next;
            delete node;
        }
        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    // Linq Functions

    TSource sum()
        requires(Computable<TSource>)
    {
        TSource total = 0;
        Node*   node  = _head;
        while (node) {
            total += node->_data;
            node = node->_next;
        }
        return total;
    }

    TSource average()
        requires(Computable<TSource>)
    {
        return sum() / count();
    }

    TSource max()
        requires(Computable<TSource>)
    {
        TSource max  = _head->data;
        Node*   node = _head->next;
        while (node) {
            if (node->_data > max) {
                max = node->_data;
            }
            node = node->_next;
        }
        return max;
    }

    TSource min()
        requires(Computable<TSource>)
    {
        TSource min  = _head->data;
        Node*   node = _head->next;
        while (node) {
            if (node->_data < min) {
                min = node->_data;
            }
            node = node->_next;
        }
        return min;
    }

    template <Computable TResult>
    TResult sum(Func<TResult(TSource const&)> mapper)
    {
        TResult total = 0;
        Node*   node  = _head;
        while (node) {
            total += mapper(node->_data);
            node = node->_next;
        }
        return total;
    }

    template <Computable TResult>
    TResult average(Func<TResult(TSource const&)> mapper)
    {
        return sum(mapper) / count();
    }

    template <Comparable TResult>
    TResult max(Func<TResult(TSource const&)> mapper)
    {
        TResult max  = mapper(_head->data);
        Node*   node = _head->next;
        while (node) {
            TResult result = mapper(node->_data);
            if (result > max) {
                max = result;
            }
            node = node->_next;
        }
        return max;
    }

    template <Comparable TResult>
    TResult min(Func<TResult(TSource const&)> mapper)
    {
        TResult min  = mapper(_head->data);
        Node*   node = _head->next;
        while (node) {
            TResult result = mapper(node->_data);
            if (result < min) {
                min = result;
            }
            node = node->_next;
        }
        return min;
    }

    template <Comparable TKey>
    TSource maxBy(Func<TKey(TSource const&)> selector)
    {
        TSource& max  = _head->data;
        TKey     key  = selector(_head->data);
        Node*    node = _head->next;
        while (node) {
            TKey result = selector(node->_data);
            if (result > key) {
                key = result;
                max = node->_data;
            }
            node = node->_next;
        }
        return max;
    }
    template <Comparable TKey>
    TSource minBy(Func<TKey(TSource const&)> selector)
    {
        TSource& min  = _head->data;
        TKey     key  = selector(_head->data);
        Node*    node = _head->next;
        while (node) {
            TKey result = selector(node->_data);
            if (result < key) {
                key = result;
                min = node->_data;
            }
            node = node->_next;
        }
        return min;
    }

    IList<TSource>& order(bool ascending = true)
        requires(Comparable<TSource>)
    {
    }

    template <Comparable TKey>
    IList<TSource>& orderBy(Func<TKey(TSource const&)> keySelector,
                            bool                       ascending)
    {
    }

    TSource findFirst() { return Std::move(_head->data); }

    TSource findLast() { return Std::move(_tail->data); }

    TSource findAny() {}

    IList<TSource>& take(usize n)
    {
        auto newList = new LinkedList<TSource>();
        if (n > 0) {
            Node* node = _head;
            while (n-- && node) {
                newList->add(node->_data);
                node = node->_next;
            }
        }
        return *newList;
    }

    IList<TSource>& take(Tuple<usize, usize> range)
    {
        auto newList = new LinkedList<TSource>();
        if (range.get<0>() > 0) {
            Node* node = _head;
            while (range.get<0>()-- && node) {
                node = node->_next;
            }
            while (range.get<1>()-- && node) {
                newList->add(node->_data);
                node = node->_next;
            }
        }
        return *newList;
    }

    IList<TSource>& takeLast(usize n)
    {
        auto newList = new LinkedList<TSource>();
        if (n > 0) {
            Node* node = _tail;
            while (n-- && node) {
                newList->pushFront(node->_data);
                node = node->_prev;
            }
        }
        return *newList;
    }

    IList<TSource>& takeWhile(Func<bool(TSource const&)> predicate)
    {
        auto  newList = new LinkedList<TSource>();
        Node* node    = _head;
        while (node && predicate(node->_data)) {
            newList->add(node->_data);
            node = node->_next;
        }
        return *newList;
    }

    TSource single()
    {
        if (_size != 1) {
            Std::panic("Sequence contains more than one element");
        }
        return Std::move(_head->data);
    }

    TSource singleOrDefault(TSource const& defaultValue)
    {
        if (_size > 1) {
            Std::panic("Sequence contains more than one element");
        }
        if (_size == 0) {
            return defaultValue;
        }
        return Std::move(_head->data);
    }

    TSource defaultIfEmpty()
        requires(Constructible<TSource>)
    {
        if (_size == 0) {
            return TSource();
        }
        return Std::move(_head->data);
    }

    TSource defaultIfEmpty(TSource const& defaultValue)
    {
        if (_size == 0) {
            return defaultValue;
        }
        return Std::move(_head->data);
    }

    IList<TSource>& skip(usize n)
    {
        auto newList = new LinkedList<TSource>();
        if (n < _size) {
            Node* node = _head;
            while (n-- && node) {
                node = node->_next;
            }
            while (node) {
                newList->add(node->_data);
                node = node->_next;
            }
        }
        return *newList;
    }

    IList<TSource>& skipLast(usize n) {}

    IList<TSource>& skipWhile(Func<bool(TSource const&)> predicate) {}

    // template <class TCollection, typename TKey>
    // TCollection<T>& groupBy(Func<TKey(TSource const&)> keySelector)
    // {
    // }

    template <typename TResult>
    IList<TResult>& select(Func<TResult(TSource const&)> selector)
    {
    }

    LinkedList<TSource>& operator+=(TSource const& data) override
    {
        add(data);
        return *this;
    }

    LinkedList<TSource>& operator-=(TSource const& data) override
    {
        remove(data);
        return *this;
    }

    TSource& operator[](usize const& index) override
    {
        if (index >= _size)
            Std::panic(Error::IndexOutOfBounds("Index out of bounds"));

        usize i    = index;
        Node* node = _head;
        while (i--) {
            node = node->_next;
        }
        return node->_data;
    }

    TSource const& operator[](usize const& index) const override
    {
        if (index >= _size)
            Std::panic(Error::IndexOutOfBounds("Index out of bounds"));

        usize i    = index;
        Node* node = _head;
        while (i--) {
            node = node->_next;
        }
        return node->_data;
    }

private:
    Node* _head = nullptr;
    Node* _tail = nullptr;
    usize _size = 0;
};