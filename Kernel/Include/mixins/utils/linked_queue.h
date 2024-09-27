#pragma once

#include <mixins/utils/collections.h>
#include <mixins/utils/linked_list.h>

template <typename TSource>
class LinkedQueue
    : public IQueue<TSource>
    , protected LinkedList<TSource>
{
public:
    TSource& Add(TSource const& e) override
    {
        LinkedList<TSource>::Add(e);
        return const_cast<TSource&>(e);
    }

    bool Remove(TSource const& e) override
    {
        if (LinkedList<TSource>::findFirst() != e)
            return false;

        LinkedList<TSource>::RemoveAt(0);
        return true;
    }

    bool Contains(TSource const& e) override
    {
        return LinkedList<TSource>::Contains(e);
    }

    usize Count() const override { return LinkedList<TSource>::Count(); }

    bool IsEmpty() const override { return LinkedList<TSource>::IsEmpty(); }

    void Clear() override { LinkedList<TSource>::Clear(); }

    void ForEach(Func<void(TSource const&)> action) const override
    {
        LinkedList<TSource>::ForEach(
            Std::forward<Func<void(TSource const&)>>(action));
    }

    IIterator<TSource>* iter() const override
    {
        return LinkedList<TSource>::iter();
    }

    TSource& Enqueue(TSource const& e) override { return Add(e); }

    TSource Dequeue() override { return LinkedList<TSource>::takeFirst(); }

    TSource peek() override { return LinkedList<TSource>::findFirst(); }
};