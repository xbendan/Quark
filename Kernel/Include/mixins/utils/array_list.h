#pragma once

#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/initializer_list.h>
#include <mixins/utils/array.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/iterator.h>

template <typename TSource>
class ArrayList : public IList<TSource>
{
    static constexpr usize DefaultCapacity = 8;

public:
    // MARK: Constructors and Destructor
    ArrayList()
        : m_data(new TSource[DefaultCapacity])
        , m_count(0)
        , m_capacity(DefaultCapacity)
    {
    }
    ArrayList(usize capacity)
        : m_data(new TSource[capacity])
        , m_count(0)
        , m_capacity(capacity)
    {
    }
    ArrayList(ArrayList<TSource> const& other)
        : m_data(new TSource[other.m_capacity])
        , m_count(other.m_count)
        , m_capacity(other.m_capacity)
    {
        for (usize i = 0; i < m_count; i++) {
            m_data[i] = Std::move(other.m_data[i]);
        }
    }
    ArrayList(ArrayList<TSource>&& other)
        : m_data(other.m_data)
        , m_count(other.m_count)
        , m_capacity(other.m_capacity)
    {
        other.m_data     = nullptr;
        other.m_count    = 0;
        other.m_capacity = 0;
    }
    template <typename... TTypes>
    ArrayList(std::initializer_list<TSource> args)
        : m_data(new TSource[args.size()])
        , m_count(args.size())
        , m_capacity(args.size())
    {
        usize i = 0;
        for (auto const& arg : args) {
            m_data[i++] = arg;
        }
    }
    ArrayList(ICollection<TSource> const& collection)
        : m_data(new TSource[collection.Count()])
        , m_count(collection.Count())
        , m_capacity(collection.Count())
    {
        collection.ForEachOrdered(
            [&](TSource const& data, usize index) { m_data[index] = data; });
    }

    ~ArrayList() { delete[] m_data; }

    ArrayList& operator=(ArrayList const& other)
    {
        if (this == &other) {
            return *this;
        }
        delete[] m_data;
        m_data     = new TSource[other.m_capacity];
        m_count    = other.m_count;
        m_capacity = other.m_capacity;
        for (usize i = 0; i < m_count; i++) {
            m_data[i] = other.m_data[i];
        }
        return *this;
    }

    ArrayList<TSource>& operator=(ArrayList<TSource>&& other)
    {
        if (this == &other) {
            return *this;
        }
        delete[] m_data;
        m_data           = other.m_data;
        m_count          = other.m_count;
        m_capacity       = other.m_capacity;
        other.m_data     = nullptr;
        other.m_count    = 0;
        other.m_capacity = 0;
        return *this;
    }

    bool operator==(ArrayList const& other) const
    {
        if (m_data == other.m_data) {
            return true;
        }

        if (m_count != other.m_count) {
            return false;
        }
        for (usize i = 0; i < m_count; i++) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }
        return true;
    }

    void Ensure(usize capacity)
    {
        if (m_capacity >= capacity) {
            return;
        }
        m_capacity    = capacity;
        TSource* data = new TSource[m_capacity];
        for (usize i = 0; i < m_count; i++) {
            data[i] = m_data[i];
        }
        delete[] m_data;
        m_data = data;
    }

    // MARK: ICollection<TSource> Implementation

    TSource& Add(TSource const& value) override
    {
        if (m_count == m_capacity) {
            m_capacity    = m_capacity * 2 + 1;
            TSource* data = new TSource[m_capacity];
            for (usize i = 0; i < m_count; i++) {
                data[i] = m_data[i];
            }
            delete[] m_data;
            m_data = data;
        }

        new (&m_data[m_count]) TSource(Std::move(value));
        return m_data[m_count - 1];
    }

    bool Remove(TSource const& value) override
    {
        for (usize i = 0; i < m_count; i++) {
            if (m_data[i] == const_cast<TSource&>(value)) {
                for (usize j = i; j < m_count - 1; j++) {
                    m_data[j] = m_data[j + 1];
                }
                m_count--;
                return true;
            }
        }
        return false;
    }

    bool Contains(TSource const& value) const override
    {
        for (usize i = 0; i < m_count; i++) {
            if (m_data[i] == value) {
                return true;
            }
        }
        return false;
    }

    usize Count() const override { return m_count; }

    bool IsEmpty() const override { return m_count == 0; }

    void Clear() override
    {
        delete[] m_data;
        m_data     = nullptr;
        m_count    = 0;
        m_capacity = 0;
    }

    ArrayList<TSource>& operator+=(TSource const& value) override
    {
        Add(value);
        return *this;
    }

    ArrayList<TSource>& operator-=(TSource const& value) override
    {
        Remove(value);
        return *this;
    }

    // MARK: IList<TSource> Implementation

    bool RemoveAt(usize index) override
    {
        if (index >= m_count) {
            return false;
        }
        for (usize i = index; i < m_count - 1; i++) {
            m_data[i] = m_data[i + 1];
        }
        m_count--;
        return true;
    }

    TSource& Insert(TSource const& value, usize index) override
    {
        if (index >= m_count) {
            return Add(value);
        }
        if (m_count == m_capacity) {
            m_capacity    = m_capacity * 2 + 1;
            TSource* data = new TSource[m_capacity];
            for (usize i = 0; i < index; i++) {
                data[i] = m_data[i];
            }
            data[index] = value;
            for (usize i = index; i < m_count; i++) {
                data[i + 1] = m_data[i];
            }
            delete[] m_data;
            m_data = data;
            m_count++;
            return m_data[index];
        }
        for (usize i = m_count; i > index; i--) {
            m_data[i] = m_data[i - 1];
        }
        m_data[index] = value;
        m_count++;
        return m_data[index];
    }

    int IndexOf(TSource const& value) const override
    {
        for (usize i = 0; i < m_count; i++) {
            if (m_data[i] == value) {
                return i;
            }
        }
        return -1;
    }

    int LastIndexOf(TSource const& value) const override
    {
        for (int i = m_count - 1; i >= 0; i--) {
            if (m_data[i] == value) {
                return i;
            }
        }
        return -1;
    }

    // MARK: IKeyedAccess<TSource> Implementation

    TSource& operator[](usize const& index) override
    {
        MakeAssertion(index < m_count,
                      Error::IndexOutOfBounds("Index out of bounds"));
        return m_data[index];
    }

    TSource const& operator[](usize const& index) const override
    {
        MakeAssertion(index < m_count,
                      Error::IndexOutOfBounds("Index out of bounds"));
        return m_data[index];
    }

    // MARK: IEnumerable<TSource> Implementation

    TSource Sum()
        requires(Computable<TSource>)
    {
        TSource result = 0;
        for (usize i = 0; i < m_count; i++) {
            result += m_data[i];
        }
        return result;
    }

    TSource Average()
        requires(Computable<TSource>)
    {
        return m_count ? (Sum() / m_count) : 0;
    }

    TSource Max()
        requires(Comparable<TSource>)
    {
        if (m_count == 0) {
            return TSource();
        }

        TSource result = m_data[0];
        for (usize i = 1; i < m_count; i++) {
            if (m_data[i] > result) {
                result = m_data[i];
            }
        }
        return result;
    }

    TSource Min()
        requires(Comparable<TSource>)
    {
        if (m_count == 0) {
            return TSource();
        }

        TSource result = m_data[0];
        for (usize i = 1; i < m_count; i++) {
            if (m_data[i] < result) {
                result = m_data[i];
            }
        }
        return result;
    }

    template <Computable TResult>
    TResult Sum(Func<TResult(TSource const&)> mapper)
    {
        TResult result = 0;
        for (usize i = 0; i < m_count; i++) {
            result += mapper(m_data[i]);
        }
        return result;
    }

    template <Computable TResult>
    TResult Average(Func<TResult(TSource const&)> mapper)
    {
        return m_count ? (Sum(mapper) / m_count) : 0;
    }

    template <Comparable TResult>
    TResult Max(Func<TResult(TSource const&)> mapper)
    {
        if (m_count == 0) {
            return TResult();
        }

        TResult result = mapper(m_data[0]);
        for (usize i = 1; i < m_count; i++) {
            TResult current = mapper(m_data[i]);
            if (current > result) {
                result = current;
            }
        }
        return result;
    }

    template <Comparable TResult>
    TResult Min(Func<TResult(TSource const&)> mapper)
    {
        if (m_count == 0) {
            return TResult();
        }

        TResult result = mapper(m_data[0]);
        for (usize i = 1; i < m_count; i++) {
            TResult current = mapper(m_data[i]);
            if (current < result) {
                result = current;
            }
        }
        return result;
    }

    template <Comparable TKey>
    TSource MaxBy(Func<TKey(TSource const&)> selector)
    {
        if (m_count == 0) {
            return TSource();
        }

        TSource result = m_data[0];
        TKey    maxKey = selector(m_data[0]);
        for (usize i = 1; i < m_count; i++) {
            TKey currentKey = selector(m_data[i]);
            if (currentKey > maxKey) {
                maxKey = currentKey;
                result = m_data[i];
            }
        }
        return result;
    }

    template <Comparable TKey>
    TSource MinBy(Func<TKey(TSource const&)> selector)
    {
        if (m_count == 0) {
            return TSource();
        }

        TSource result = m_data[0];
        TKey    minKey = selector(m_data[0]);
        for (usize i = 1; i < m_count; i++) {
            TKey currentKey = selector(m_data[i]);
            if (currentKey < minKey) {
                minKey = currentKey;
                result = m_data[i];
            }
        }
        return result;
    }

    ArrayList<TSource>& Order(bool ascending = true)
        requires(Comparable<TSource>)
    {
    }

    template <Comparable TKey>
    ArrayList<TSource>& OrderBy(Func<TKey(TSource const&)> keySelector,
                                bool                       ascending)
    {
    }

    Opt<TSource&> FindFirst() override
    {
        if (m_count == 0) {
            return Empty();
        }

        return m_data[0];
    }

    Opt<TSource&> FindFirst(Predicate<TSource const&> predicate) override
    {
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                return m_data[i];
            }
        }
        return Empty();
    }

    Opt<TSource&> FindLast() override
    {
        if (m_count == 0) {
            return Empty();
        }

        return m_data[m_count - 1];
    }

    Opt<TSource&> FindLast(Predicate<TSource const&> predicate) override
    {
        for (int i = m_count - 1; i >= 0; i--) {
            if (predicate(m_data[i])) {
                return m_data[i];
            }
        }
        return Empty();
    }

    Opt<TSource&> FindAny() override
    {
        if (m_count == 0) {
            return Empty();
        }

        return m_data[0];
    }

    Opt<TSource&> FindAny(Predicate<TSource const&> predicate) override
    {
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                return m_data[i];
            }
        }
        return Empty();
    }

    IList<TSource>& Take(usize n) override
    {
        ArrayList* newList = new ArrayList<TSource>(n);
        for (usize i = 0; i < n && i < m_count; i++) {
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    IList<TSource>& Take(Tuple<usize, usize> range) override
    {
        auto newList = new ArrayList<TSource>(range.v1 - range.v0);
        for (usize i = range.v0; i < range.v1 && i < m_count; i++) {
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    IList<TSource>& TakeLast(usize n) override
    {
        auto newList = new ArrayList<TSource>(n);
        for (usize i = m_count - n; i < m_count; i++) {
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    IList<TSource>& TakeWhile(Predicate<TSource const&> predicate) override
    {
        auto newList = new ArrayList<TSource>();
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                newList->Add(m_data[i]);
            }
        }
        return *newList;
    }

    TSource& Single() override
    {
        MakeAssertion(
            m_count == 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return m_data[0];
    }

    TSource& Single(Predicate<TSource const&> predicate)
    {
        usize index = -1;
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                if (index != -1) {
                    MakeAssertion(
                        false,
                        Error::InvalidOperation(
                            "Sequence contains more than one element"));
                }
                index = i;
            }
        }
        MakeAssertion(index != -1,
                      Error::InvalidOperation("Sequence contains no element"));
        return m_data[index];
    }

    TSource& SingleOrDefault(TSource const& defaultValue) override
    {
        MakeAssertion(
            m_count <= 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return m_count ? m_data[0] : const_cast<TSource&>(defaultValue);
    }

    TSource& DefaultIfEmpty()
        requires(Constructible<TSource>)
    {
        return m_count ? m_data[0] : TSource();
    }

    TSource& DefaultIfEmpty(TSource const& defaultValue) override
    {
        return m_count ? m_data[0] : const_cast<TSource&>(defaultValue);
    }

    IList<TSource>& Skip(usize n) override
    {
        auto newList = new ArrayList<TSource>(m_count - n);
        for (usize i = n; i < m_count; i++) {
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    IList<TSource>& SkipLast(usize n) override
    {
        auto newList = new ArrayList<TSource>(m_count - n);
        for (usize i = 0; i < m_count - n; i++) {
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    IList<TSource>& SkipWhile(Predicate<TSource const&> predicate) override
    {
        auto newList = new ArrayList<TSource>();
        for (usize i = 0; i < m_count; i++) {
            if (!predicate(m_data[i])) {
                continue;
            }
            newList->Add(m_data[i]);
        }
        return *newList;
    }

    // template <class TCollection, typename TKey>
    // TCollection<TSource>& groupBy(Func<TKey(TSource const&)> keySelector)
    // {
    // }

    template <typename TResult>
    ArrayList<TResult>& Select(Func<TResult(TSource const&)> selector)
    {
        auto newList = new ArrayList<TResult>(m_count);
        for (usize i = 0; i < m_count; i++) {
            newList->Add(selector(m_data[i]));
        }
        return *newList;
    }

    bool AllMatch(Predicate<TSource const&> predicate) const override
    {
        for (usize i = 0; i < m_count; i++) {
            if (!predicate(m_data[i])) {
                return false;
            }
        }
        return true;
    }

    bool AnyMatch(Predicate<TSource const&> predicate) const override
    {
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                return true;
            }
        }
        return false;
    }

    bool NoneMatch(Predicate<TSource const&> predicate) const override
    {
        for (usize i = 0; i < m_count; i++) {
            if (predicate(m_data[i])) {
                return false;
            }
        }
        return true;
    }

    void ForEach(Func<void(TSource const&)> action) const override
    {
        for (usize i = 0; i < m_count; i++) {
            action(m_data[i]);
        }
    }

    void ForEachOrdered(Func<void(TSource const&, usize)> action) const override
    {
        for (usize i = 0; i < m_count; i++) {
            action(m_data[i], i);
        }
    }

private:
    TSource* m_data;
    usize    m_count;
    usize    m_capacity;
};