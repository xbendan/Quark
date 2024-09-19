#pragma once

#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/initializer_list.h>
#include <mixins/utils/array.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/iterator.h>

template <typename TSource>
class ArrayList
    : public IList<TSource>
    , public IReadOnlyList<TSource>
{
    static constexpr usize DefaultCapacity = 8;

public:
    // MARK: Constructors and Destructor
    ArrayList()
        : _data(new TSource[DefaultCapacity])
        , _count(0)
        , _capacity(DefaultCapacity)
    {
    }
    ArrayList(usize capacity)
        : _data(new TSource[capacity])
        , _count(0)
        , _capacity(capacity)
    {
    }
    ArrayList(ArrayList<TSource> const& other)
        : _data(new TSource[other._capacity])
        , _count(other._count)
        , _capacity(other._capacity)
    {
        for (usize i = 0; i < _count; i++) {
            _data[i] = Std::move(other._data[i]);
        }
    }
    ArrayList(ArrayList<TSource>&& other)
        : _data(other._data)
        , _count(other._count)
        , _capacity(other._capacity)
    {
        other._data     = nullptr;
        other._count    = 0;
        other._capacity = 0;
    }
    template <typename... TTypes>
    ArrayList(std::initializer_list<TSource> args)
        : _data(new TSource[args.size()])
        , _count(args.size())
        , _capacity(args.size())
    {
        usize i = 0;
        for (auto const& arg : args) {
            _data[i++] = arg;
        }
    }
    ArrayList(IReadOnlyCollection<TSource> const& collection)
        : _data(new TSource[collection.count()])
        , _count(collection.count())
        , _capacity(collection.count())
    {
        usize    i    = 0;
        auto&    iter = collection.iter();
        TSource& data = iter.current();
        while (true) {
            _data[i++] = data;
            if (!iter.hasNext()) {
                break;
            }
            data = iter.next();
        }

        delete &iter;
    }

    ~ArrayList() { delete[] _data; }

    ArrayList& operator=(ArrayList const& other)
    {
        if (this == &other) {
            return *this;
        }
        delete[] _data;
        _data     = new TSource[other._capacity];
        _count    = other._count;
        _capacity = other._capacity;
        for (usize i = 0; i < _count; i++) {
            _data[i] = other._data[i];
        }
        return *this;
    }

    ArrayList<TSource>& operator=(ArrayList<TSource>&& other)
    {
        if (this == &other) {
            return *this;
        }
        delete[] _data;
        _data           = other._data;
        _count          = other._count;
        _capacity       = other._capacity;
        other._data     = nullptr;
        other._count    = 0;
        other._capacity = 0;
        return *this;
    }

    // MARK: ICollection<TSource> Implementation

    TSource& add(TSource const& value) override
    {
        if (_count == _capacity) {
            _capacity     = _capacity * 2 + 1;
            TSource* data = new TSource[_capacity];
            for (usize i = 0; i < _count; i++) {
                data[i] = _data[i];
            }
            delete[] _data;
            _data = data;
        }
        _data[_count++] = Std::move(value);
        return _data[_count - 1];
    }

    bool remove(TSource const& value) override
    {
        for (usize i = 0; i < _count; i++) {
            if (_data[i] == const_cast<TSource&>(value)) {
                for (usize j = i; j < _count - 1; j++) {
                    _data[j] = _data[j + 1];
                }
                _count--;
                return true;
            }
        }
        return false;
    }

    bool contains(TSource const& value) override
    {
        for (usize i = 0; i < _count; i++) {
            if (_data[i] == value) {
                return true;
            }
        }
        return false;
    }

    bool contains(TSource const& value) const override
    {
        for (usize i = 0; i < _count; i++) {
            if (_data[i] == value) {
                return true;
            }
        }
        return false;
    }

    usize count() const override { return _count; }

    bool isEmpty() const override { return _count == 0; }

    void clear() override
    {
        delete[] _data;
        _data     = nullptr;
        _count    = 0;
        _capacity = 0;
    }

    ArrayList<TSource>& operator+=(TSource const& value) override
    {
        add(value);
        return *this;
    }

    ArrayList<TSource>& operator-=(TSource const& value) override
    {
        remove(value);
        return *this;
    }

    // MARK: IList<TSource> Implementation

    bool removeAt(usize index) override
    {
        if (index >= _count) {
            return false;
        }
        for (usize i = index; i < _count - 1; i++) {
            _data[i] = _data[i + 1];
        }
        _count--;
        return true;
    }

    TSource& insert(TSource const& value, usize index) override
    {
        if (index >= _count) {
            return add(value);
        }
        if (_count == _capacity) {
            _capacity     = _capacity * 2 + 1;
            TSource* data = new TSource[_capacity];
            for (usize i = 0; i < index; i++) {
                data[i] = _data[i];
            }
            data[index] = value;
            for (usize i = index; i < _count; i++) {
                data[i + 1] = _data[i];
            }
            delete[] _data;
            _data = data;
            _count++;
            return _data[index];
        }
        for (usize i = _count; i > index; i--) {
            _data[i] = _data[i - 1];
        }
        _data[index] = value;
        _count++;
        return _data[index];
    }

    int indexOf(TSource const& value) const override
    {
        for (usize i = 0; i < _count; i++) {
            if (_data[i] == value) {
                return i;
            }
        }
        return -1;
    }

    int lastIndexOf(TSource const& value) const override
    {
        for (int i = _count - 1; i >= 0; i--) {
            if (_data[i] == value) {
                return i;
            }
        }
        return -1;
    }

    // MARK: IKeyedAccess<TSource> Implementation

    TSource& operator[](usize const& index) override
    {
        assert(index < _count, Error::IndexOutOfBounds("Index out of bounds"));
        return _data[index];
    }

    TSource const& operator[](usize const& index) const override
    {
        assert(index < _count, Error::IndexOutOfBounds("Index out of bounds"));
        return _data[index];
    }

    IIterator<TSource>& iter() const override
    {
        return *new ArrayIterator<TSource>(_data, _count);
    }

    // MARK: IEnumerable<TSource> Implementation

    TSource sum()
        requires(Computable<TSource>)
    {
        TSource result = 0;
        for (usize i = 0; i < _count; i++) {
            result += _data[i];
        }
        return result;
    }

    TSource average()
        requires(Computable<TSource>)
    {
        return _count ? (sum() / _count) : 0;
    }

    TSource max()
        requires(Comparable<TSource>)
    {
        if (_count == 0) {
            return TSource();
        }

        TSource result = _data[0];
        for (usize i = 1; i < _count; i++) {
            if (_data[i] > result) {
                result = _data[i];
            }
        }
        return result;
    }

    TSource min()
        requires(Comparable<TSource>)
    {
        if (_count == 0) {
            return TSource();
        }

        TSource result = _data[0];
        for (usize i = 1; i < _count; i++) {
            if (_data[i] < result) {
                result = _data[i];
            }
        }
        return result;
    }

    template <Computable TResult>
    TResult sum(Func<TResult(TSource const&)> mapper)
    {
        TResult result = 0;
        for (usize i = 0; i < _count; i++) {
            result += mapper(_data[i]);
        }
        return result;
    }

    template <Computable TResult>
    TResult average(Func<TResult(TSource const&)> mapper)
    {
        return _count ? (sum(mapper) / _count) : 0;
    }

    template <Comparable TResult>
    TResult max(Func<TResult(TSource const&)> mapper)
    {
        if (_count == 0) {
            return TResult();
        }

        TResult result = mapper(_data[0]);
        for (usize i = 1; i < _count; i++) {
            TResult current = mapper(_data[i]);
            if (current > result) {
                result = current;
            }
        }
        return result;
    }

    template <Comparable TResult>
    TResult min(Func<TResult(TSource const&)> mapper)
    {
        if (_count == 0) {
            return TResult();
        }

        TResult result = mapper(_data[0]);
        for (usize i = 1; i < _count; i++) {
            TResult current = mapper(_data[i]);
            if (current < result) {
                result = current;
            }
        }
        return result;
    }

    template <Comparable TKey>
    TSource maxBy(Func<TKey(TSource const&)> selector)
    {
        if (_count == 0) {
            return TSource();
        }

        TSource result = _data[0];
        TKey    maxKey = selector(_data[0]);
        for (usize i = 1; i < _count; i++) {
            TKey currentKey = selector(_data[i]);
            if (currentKey > maxKey) {
                maxKey = currentKey;
                result = _data[i];
            }
        }
        return result;
    }

    template <Comparable TKey>
    TSource minBy(Func<TKey(TSource const&)> selector)
    {
        if (_count == 0) {
            return TSource();
        }

        TSource result = _data[0];
        TKey    minKey = selector(_data[0]);
        for (usize i = 1; i < _count; i++) {
            TKey currentKey = selector(_data[i]);
            if (currentKey < minKey) {
                minKey = currentKey;
                result = _data[i];
            }
        }
        return result;
    }

    ArrayList<TSource>& order(bool ascending = true)
        requires(Comparable<TSource>)
    {
    }

    template <Comparable TKey>
    ArrayList<TSource>& orderBy(Func<TKey(TSource const&)> keySelector,
                                bool                       ascending)
    {
    }

    TSource findFirst() { return _count ? _data[0] : TSource(); }

    TSource findLast() { return _count ? _data[_count - 1] : TSource(); }

    TSource findAny() { return _count ? _data[0] : TSource(); }

    ArrayList<TSource>& take(usize n)
    {
        auto newList = new ArrayList<TSource>(n);
        for (usize i = 0; i < n && i < _count; i++) {
            newList->add(_data[i]);
        }
        return *newList;
    }

    ArrayList<TSource>& take(Tuple<usize, usize> range)
    {
        auto newList = new ArrayList<TSource>(range.get<1>() - range.get<0>());
        for (usize i = range.get<0>(); i < range.get<1>() && i < _count; i++) {
            newList->add(_data[i]);
        }
        return *newList;
    }

    ArrayList<TSource>& takeLast(usize n)
    {
        auto newList = new ArrayList<TSource>(n);
        for (usize i = _count - n; i < _count; i++) {
            newList->add(_data[i]);
        }
        return *newList;
    }

    ArrayList<TSource>& takeWhile(Predicate<TSource const&> predicate)
    {
        auto newList = new ArrayList<TSource>();
        for (usize i = 0; i < _count; i++) {
            if (predicate(_data[i])) {
                newList->add(_data[i]);
            }
        }
        return *newList;
    }

    TSource& single()
    {
        assert(
            _count == 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return _data[0];
    }

    TSource& single(Predicate<TSource const&> predicate)
    {
        usize index = -1;
        for (usize i = 0; i < _count; i++) {
            if (predicate(_data[i])) {
                if (index != -1) {
                    assert(false,
                           Error::InvalidOperation(
                               "Sequence contains more than one element"));
                }
                index = i;
            }
        }
        assert(index != -1,
               Error::InvalidOperation("Sequence contains no element"));
        return _data[index];
    }

    TSource& singleOrDefault(TSource const& defaultValue)
    {
        assert(
            _count <= 1,
            Error::InvalidOperation("Sequence contains more than one element"));
        return _count ? _data[0] : defaultValue;
    }

    TSource& defaultIfEmpty()
        requires(Constructible<TSource>)
    {
        return _count ? _data[0] : TSource();
    }

    TSource& defaultIfEmpty(TSource const& defaultValue)
    {
        return _count ? _data[0] : defaultValue;
    }

    ArrayList<TSource>& skip(usize n)
    {
        auto newList = new ArrayList<TSource>(_count - n);
        for (usize i = n; i < _count; i++) {
            newList->add(_data[i]);
        }
        return *newList;
    }

    ArrayList<TSource>& skipLast(usize n)
    {
        auto newList = new ArrayList<TSource>(_count - n);
        for (usize i = 0; i < _count - n; i++) {
            newList->add(_data[i]);
        }
        return *newList;
    }

    ArrayList<TSource>& skipWhile(Predicate<TSource const&> predicate)
    {
        auto newList = new ArrayList<TSource>();
        for (usize i = 0; i < _count; i++) {
            if (!predicate(_data[i])) {
                continue;
            }
            newList->add(_data[i]);
        }
        return *newList;
    }

    // template <class TCollection, typename TKey>
    // TCollection<TSource>& groupBy(Func<TKey(TSource const&)> keySelector)
    // {
    // }

    template <typename TResult>
    ArrayList<TResult>& select(Func<TResult(TSource const&)> selector)
    {
        auto newList = new ArrayList<TResult>(_count);
        for (usize i = 0; i < _count; i++) {
            newList->add(selector(_data[i]));
        }
        return *newList;
    }

    bool allMatch(Predicate<TSource const&> predicate) const
    {
        for (usize i = 0; i < _count; i++) {
            if (!predicate(_data[i])) {
                return false;
            }
        }
        return true;
    }

    bool anyMatch(Predicate<TSource const&> predicate) const
    {
        for (usize i = 0; i < _count; i++) {
            if (predicate(_data[i])) {
                return true;
            }
        }
        return false;
    }

    bool noneMatch(Predicate<TSource const&> predicate) const
    {
        for (usize i = 0; i < _count; i++) {
            if (predicate(_data[i])) {
                return false;
            }
        }
        return true;
    }

    void forEach(Func<void(TSource const&)> action) const override
    {
        for (usize i = 0; i < _count; i++) {
            action(_data[i]);
        }
    }

    void forEachOrdered(Func<void(TSource const&, usize)> action) const override
    {
        for (usize i = 0; i < _count; i++) {
            action(_data[i], i);
        }
    }

private:
    TSource* _data;
    usize    _count;
    usize    _capacity;
};