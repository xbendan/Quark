#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/concepts.h>

template <typename TSource>
class IIterable
{
    virtual TSource* begin() const = 0;
    virtual TSource* end() const   = 0;
};

template <typename TSource>
class Array;

template <typename TSource>
class IList;

template <typename TSource>
class IEnumerable
{
public:
    //     virtual TSource sum()
    //         requires(Computable<TSource>)
    //     = 0;
    //     virtual TSource average()
    //         requires(Computable<TSource>)
    //     = 0;

    //     virtual TSource max()
    //         requires(Comparable<TSource>)
    //     = 0;
    //     virtual TSource min()
    //         requires(Comparable<TSource>)
    //     = 0;

    //     template <Computable TResult>
    //     virtual TResult sum(Func<TResult(TSource const&)> mapper) = 0;
    //     template <Computable TResult>
    //     virtual TResult average(Func<TResult(TSource const&)> mapper) = 0;
    //     template <Comparable TResult>
    //     virtual TResult max(Func<TResult(TSource const&)> mapper) = 0;
    //     template <Comparable TResult>
    //     virtual TResult min(Func<TResult(TSource const&)> mapper) = 0;

    //     template <Comparable TKey>
    //     virtual TSource maxBy(Func<TKey(TSource const&)> selector) = 0;
    //     template <Comparable TKey>
    //     virtual TSource minBy(Func<TKey(TSource const&)> selector) = 0;

    //     virtual IEnumerable<TSource>& order(bool ascending = true)
    //         requires(Comparable<TSource>)
    //     = 0;
    //     template <Comparable TKey>
    //     virtual IEnumerable<TSource>& orderBy(
    //         Func<TKey(TSource const&)> keySelector,
    //         bool                       ascending) = 0;

    virtual TSource findFirst() = 0;
    virtual TSource findLast()  = 0;
    virtual TSource findAny()   = 0;

    virtual IEnumerable<TSource>& take(usize n)                   = 0;
    virtual IEnumerable<TSource>& take(Tuple<usize, usize> range) = 0;
    virtual IEnumerable<TSource>& takeLast(usize n)               = 0;
    virtual IEnumerable<TSource>& takeWhile(
        Func<bool(TSource const&)> predicate) = 0;

    //     virtual TSource single()                                     = 0;
    //     virtual TSource singleOrDefault(TSource const& defaultValue) = 0;
    //     virtual TSource defaultIfEmpty()
    //         requires(Constructible<TSource>)
    //     = 0;
    //     virtual TSource defaultIfEmpty(TSource const& defaultValue) = 0;

    //     virtual IEnumerable<TSource>& skip(usize n)     = 0;
    //     virtual IEnumerable<TSource>& skipLast(usize n) = 0;
    //     virtual IEnumerable<TSource>& skipWhile(
    //         Func<bool(TSource const&)> predicate) = 0;

    //     template <class TCollection, typename TKey>
    //     virtual TCollection<T>& groupBy(Func<TKey(TSource const&)>
    //     keySelector) = 0;

    //     template <typename TResult>
    //     virtual IEnumerable<TResult>& select(
    //         Func<TResult(TSource const&)> selector) = 0;

    //     virtual bool  contains(T const& e) = 0;
    //     virtual usize count()              = 0;

    virtual bool allMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool anyMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool noneMatch(Func<bool(TSource const&)> predicate) const = 0;

    virtual void forEach(Func<void(TSource const&)> action) = 0;

    //     virtual Array<TSource>& toArray() = 0;
    //     virtual IList<TSource>& toList()  = 0;
    //     virtual ISet<TSource>& toSet()    = 0;
};

template <typename TSource>
class IIterator
{
public:
    virtual ~IIterator() = default;

    virtual TSource& current() const = 0;
    virtual TSource& next()          = 0;
    virtual bool     hasNext() const = 0;

    virtual TSource& operator++()
    {
        next();
        return current();
    }
};

template <typename TSource>
class ICollection
{
public:
    virtual TSource& add(TSource const& e)      = 0;
    virtual bool     remove(TSource const& e)   = 0;
    virtual bool     contains(TSource const& e) = 0;
    virtual usize    count() const              = 0;
    virtual bool     isEmpty() const            = 0;
    virtual void     clear()                    = 0;

    virtual void forEach(Func<void(TSource const&)> action) const = 0;

    virtual IIterator<TSource>& iter() const = 0;

    virtual ICollection<TSource>& operator+=(TSource const& e)
    {
        add(e);
        return *this;
    }

    virtual ICollection<TSource>& operator-=(TSource const& e)
    {
        remove(e);
        return *this;
    }
};

template <typename TSource>
class IReadOnlyCollection
{
public:
    virtual bool  contains(TSource const& e) const = 0;
    virtual usize count() const                    = 0;
    virtual bool  isEmpty() const                  = 0;

    virtual void forEach(Func<void(TSource const&)> action) const = 0;

    virtual IIterator<TSource>& iter() const = 0;
};

template <typename TKey, typename TValue>
class IKeyedAccess
{
public:
    virtual TValue&       operator[](TKey const& key)       = 0;
    virtual TValue const& operator[](TKey const& key) const = 0;
};

template <typename TSource>
class IList
    : public ICollection<TSource>
    , public IKeyedAccess<usize, TSource>
{
public:
    // virtual TSource& pushBack(TSource const& e)          = 0;
    // virtual TSource& pushFront(TSource const& e)         = 0;
    // virtual TSource  popBack()                           = 0;
    // virtual TSource  popFront()                          = 0;
    virtual bool     remove(TSource const& e)            = 0;
    virtual bool     removeAt(usize i)                   = 0;
    virtual TSource& insert(TSource const& e, usize i)   = 0;
    virtual int      indexOf(TSource const& e) const     = 0;
    virtual int      lastIndexOf(TSource const& e) const = 0;

    virtual void forEachOrdered(
        Func<void(TSource const&, usize)> action) const = 0;
};

template <typename TSource>
class IReadOnlyList
    : public IReadOnlyCollection<TSource>
    , public IKeyedAccess<usize, TSource>
{
public:
    virtual int indexOf(TSource const& e) const     = 0;
    virtual int lastIndexOf(TSource const& e) const = 0;

    virtual void forEachOrdered(
        Func<void(TSource const&, usize)> action) const = 0;
};

template <typename TSource>
class ISet : public ICollection<TSource>
{
public:
    virtual bool           isSubsetOf(ISet<TSource> const& other) const   = 0;
    virtual bool           isSupersetOf(ISet<TSource> const& other) const = 0;
    virtual ISet<TSource>& isOverlapWith(ISet<TSource> const& other)      = 0;

    virtual ISet<TSource>& exceptWith(ISet<TSource> const& other)          = 0;
    virtual ISet<TSource>& unionWith(ISet<TSource> const& other)           = 0;
    virtual ISet<TSource>& intersectWith(ISet<TSource> const& other)       = 0;
    virtual ISet<TSource>& symmetricExceptWith(ISet<TSource> const& other) = 0;
};

template <typename TSource>
class IQueue : public ICollection<TSource>
{
    virtual TSource& enqueue(TSource const& e) = 0;
    virtual TSource  dequeue()                 = 0;
    virtual TSource  peek()                    = 0;
};

// template <typename TSource, typename E>
// concept Queue = requires(TSource& t, E const& e) {
//     { t.enqueue(e) } -> SameAs<TSource&>;
//     { t.dequeue() } -> SameAs<E>;
//     { t.peek() } -> SameAs<E>;
//     { t.isEmpty() } -> ConvertibleTo<bool>;
//     { t.size() } -> ConvertibleTo<usize>;
//     { t.clear() } -> SameAs<void>;
// };