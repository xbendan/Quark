#pragma once

#include <mixins/meta/func.h>
#include <mixins/meta/opt.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/concepts.h>

template <typename TSource>
class IIterable
{
    virtual TSource* begin() const = 0;
    virtual TSource* end() const   = 0;
};

template <typename TSource>
    requires(Computable<TSource> && Comparable<TSource>)
class Enumerable
{
public:
    virtual TSource Sum()     = 0;
    virtual TSource Average() = 0;
    virtual TSource Max()     = 0;
    virtual TSource Min()     = 0;

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

    virtual TSource& findFirst() = 0;
    virtual TSource& findLast()  = 0;
    virtual TSource& findAny()   = 0;

    virtual Enumerable<TSource>& take(usize n)                   = 0;
    virtual Enumerable<TSource>& take(Tuple<usize, usize> range) = 0;
    virtual Enumerable<TSource>& takeLast(usize n)               = 0;
    virtual Enumerable<TSource>& takeWhile(
        Func<bool(TSource const&)> predicate) = 0;

    virtual TSource single()                                     = 0;
    virtual TSource singleOrDefault(TSource const& defaultValue) = 0;
    // virtual TSource defaultIfEmpty()
    //     requires(Constructible<TSource>)
    // = 0;
    virtual TSource defaultIfEmpty(TSource const& defaultValue)  = 0;

    virtual Enumerable<TSource>& skip(usize n)     = 0;
    virtual Enumerable<TSource>& skipLast(usize n) = 0;
    virtual Enumerable<TSource>& skipWhile(
        Func<bool(TSource const&)> predicate) = 0;

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

    virtual void forEach(Func<void(TSource const&)> action) const = 0;

    //     virtual Array<TSource>& toArray() = 0;
    //     virtual IList<TSource>& toList()  = 0;
    //     virtual ISet<TSource>& toSet()    = 0;
};

// template <template <typename> typename TClass, typename TSource>
// concept IEnumerable =
//     requires(TClass<TSource>                   t,
//              TSource const&                    e,
//              Predicate<TSource const&>         predicate,
//              Action<TSource const&>            action,
//              Func<void(TSource const&, usize)> actionIndexed) {
//         { t.FindFirst() } -> SameAs<Optional<TSource&>>;
//         { t.FindLast() } -> SameAs<Optional<TSource&>>;
//         { t.FindAny() } -> SameAs<Optional<TSource&>>;
//         { t.Take(0) } -> SameAs<TClass<TSource>&>;
//         { t.Take(Tuple<usize, usize>()) } -> SameAs<TClass<TSource>&>;
//         { t.TakeLast(0) } -> SameAs<TClass<TSource>&>;
//         { t.TakeWhile(predicate) } -> SameAs<TClass<TSource>&>;
//         { t.Skip(0) } -> SameAs<TClass<TSource>&>;
//         { t.SkipLast(0) } -> SameAs<TClass<TSource>&>;
//         { t.SkipWhile(predicate) } -> SameAs<TClass<TSource>&>;
//         { t.Single() } -> SameAs<TSource&>;
//         { t.Single(predicate) } -> SameAs<TSource&>;
//         { t.SingleOrDefault(e) } -> SameAs<TSource&>;
//         { t.DefaultIfEmpty(e) } -> SameAs<TSource&>;
//         { t.AllMatch(predicate) } -> ConvertibleTo<bool>;
//         { t.AnyMatch(predicate) } -> ConvertibleTo<bool>;
//         { t.NoneMatch(predicate) } -> ConvertibleTo<bool>;
//         { t.ForEach(action) } -> SameAs<void>;
//         { t.ForEachOrdered(actionIndexed) } -> SameAs<void>;
//     };

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
class IEnumerable
{
public:
    virtual Opt<TSource&> FindFirst()                                    = 0;
    virtual Opt<TSource&> FindFirst(Predicate<TSource const&> predicate) = 0;
    virtual Opt<TSource&> FindLast()                                     = 0;
    virtual Opt<TSource&> FindLast(Predicate<TSource const&> predicate)  = 0;
    virtual Opt<TSource&> FindAny()                                      = 0;
    virtual Opt<TSource&> FindAny(Predicate<TSource const&> predicate)   = 0;
    virtual IEnumerable<TSource>& Take(usize n)                          = 0;
    virtual IEnumerable<TSource>& Take(Tuple<usize, usize> r)            = 0;
    virtual IEnumerable<TSource>& TakeLast(usize n)                      = 0;
    virtual IEnumerable<TSource>& TakeWhile(
        Func<bool(TSource const&)> predicate)       = 0;
    virtual IEnumerable<TSource>& Skip(usize n)     = 0;
    virtual IEnumerable<TSource>& SkipLast(usize n) = 0;
    virtual IEnumerable<TSource>& SkipWhile(
        Func<bool(TSource const&)> predicate)                              = 0;
    virtual TSource& Single()                                              = 0;
    virtual TSource& SingleOrDefault(TSource const& defaultValue)          = 0;
    virtual TSource& DefaultIfEmpty(TSource const& defaultValue)           = 0;
    virtual bool     AllMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool     AnyMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool     NoneMatch(Func<bool(TSource const&)> predicate) const = 0;
    virtual void     ForEach(Func<void(TSource const&)> action) const      = 0;
    virtual void     ForEachOrdered(
            Func<void(TSource const&, usize)> action) const = 0;
};

template <typename TSource>
    requires(Computable<TSource> && Comparable<TSource>)
class IEnumerable<TSource>
{
public:
    virtual TSource               Sum()                        = 0;
    virtual TSource               Average()                    = 0;
    virtual TSource               Max()                        = 0;
    virtual TSource               Min()                        = 0;
    virtual IEnumerable<TSource>& Order(bool ascending = true) = 0;

    virtual Opt<TSource&> FindFirst()                                    = 0;
    virtual Opt<TSource&> FindFirst(Predicate<TSource const&> predicate) = 0;
    virtual Opt<TSource&> FindLast()                                     = 0;
    virtual Opt<TSource&> FindLast(Predicate<TSource const&> predicate)  = 0;
    virtual Opt<TSource&> FindAny()                                      = 0;
    virtual Opt<TSource&> FindAny(Predicate<TSource const&> predicate)   = 0;
    virtual IEnumerable<TSource>& Take(usize n)                          = 0;
    virtual IEnumerable<TSource>& Take(Tuple<usize, usize> r)            = 0;
    virtual IEnumerable<TSource>& TakeLast(usize n)                      = 0;
    virtual IEnumerable<TSource>& TakeWhile(
        Func<bool(TSource const&)> predicate)       = 0;
    virtual IEnumerable<TSource>& Skip(usize n)     = 0;
    virtual IEnumerable<TSource>& SkipLast(usize n) = 0;
    virtual IEnumerable<TSource>& SkipWhile(
        Func<bool(TSource const&)> predicate)                              = 0;
    virtual TSource& Single()                                              = 0;
    virtual TSource& SingleOrDefault(TSource const& defaultValue)          = 0;
    virtual TSource& DefaultIfEmpty(TSource const& defaultValue)           = 0;
    virtual bool     AllMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool     AnyMatch(Func<bool(TSource const&)> predicate) const  = 0;
    virtual bool     NoneMatch(Func<bool(TSource const&)> predicate) const = 0;
    virtual void     ForEach(Func<void(TSource const&)> action) const      = 0;
    virtual void     ForEachOrdered(
            Func<void(TSource const&, usize)> action) const = 0;
};

template <typename TSource>
class ICollection : public IEnumerable<TSource>
{
public:
    virtual TSource& Add(TSource const& e)            = 0;
    virtual bool     Remove(TSource const& e)         = 0;
    virtual bool     Contains(TSource const& e) const = 0;
    virtual usize    Count() const                    = 0;
    virtual bool     IsEmpty() const                  = 0;
    virtual void     Clear()                          = 0;

    virtual ICollection<TSource>& operator+=(TSource const& e)
    {
        Add(e);
        return *this;
    }

    virtual ICollection<TSource>& operator-=(TSource const& e)
    {
        Remove(e);
        return *this;
    }
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
    virtual bool     Contains(TSource const& e) const    = 0;
    virtual usize    Count() const                       = 0;
    virtual bool     IsEmpty() const                     = 0;
    virtual bool     RemoveAt(usize i)                   = 0;
    virtual TSource& Insert(TSource const& e, usize i)   = 0;
    virtual int      IndexOf(TSource const& e) const     = 0;
    virtual int      LastIndexOf(TSource const& e) const = 0;
};

template <typename TSource>
class ISet : public ICollection<TSource>
{
public:
    virtual bool           IsSubsetOf(ISet<TSource> const& other) const   = 0;
    virtual bool           IsSupersetOf(ISet<TSource> const& other) const = 0;
    virtual ISet<TSource>& IsOverlapWith(ISet<TSource> const& other)      = 0;

    virtual ISet<TSource>& ExceptWith(ISet<TSource> const& other)          = 0;
    virtual ISet<TSource>& UnionWith(ISet<TSource> const& other)           = 0;
    virtual ISet<TSource>& IntersectWith(ISet<TSource> const& other)       = 0;
    virtual ISet<TSource>& SymmetricExceptWith(ISet<TSource> const& other) = 0;
};

template <typename TSource>
class IQueue
{
    virtual TSource&      Enqueue(TSource const& e) = 0;
    virtual Opt<TSource>  Dequeue()                 = 0;
    virtual Opt<TSource&> Peek()                    = 0;
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