#pragma once

#include <mixins/utils/collections.h>
#include <mixins/utils/linked_list.h>

template <typename TSource>
class LinkedQueue
    : public IQueue<TSource>
    , protected LinkedList<TSource>
{
public:
    TSource& add(TSource const& e) override
    {
        LinkedList<TSource>::add(e);
        return const_cast<TSource&>(e);
    }

    bool remove(TSource const& e) override
    {
        if (LinkedList<TSource>::findFirst() != e)
            return false;

        LinkedList<TSource>::removeAt(0);
        return true;
    }

    bool contains(TSource const& e) override
    {
        return LinkedList<TSource>::contains(e);
    }

    usize count() const override { return LinkedList<TSource>::count(); }

    bool isEmpty() const override { return LinkedList<TSource>::isEmpty(); }

    void clear() override { LinkedList<TSource>::clear(); }

    IIterator<TSource>& iter() const override
    {
        return LinkedList<TSource>::iter();
    }

    TSource& enqueue(TSource const& e) override { return add(e); }

    TSource dequeue() override { return LinkedList<TSource>::takeFirst(); }

    TSource peek() override { return LinkedList<TSource>::findFirst(); }
};