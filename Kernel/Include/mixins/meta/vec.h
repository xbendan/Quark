#include <mixins/meta/buf.h>
#include <mixins/meta/slice.h>
#include <mixins/std/c++types.h>
#include <mixins/std/panic.h>
#include <mixins/std/utility.h>

template <typename S, typename T = typename S::InnerType>
struct _Vec
{
    using Inner = T;

    S _buf{};

    constexpr _Vec() = default;

    _Vec(usize cap)
        : _buf(cap)
    {
    }

    _Vec(Sliceable<T> auto const& other)
        : _buf(other)
    {
    }

    _Vec(S storage)
        : _buf(storage)
    {
    }

    // MARK: Collection

    bool contains(T const& val) const
    {
        for (auto const& v : *this) {
            if (v == val) {
                return true;
            }
        }

        return false;
    }

    bool removeAll(T const& val)
    {
        bool changed = false;

        for (usize i = 1; i < _buf.len() + 1; i++) {
            if (_buf[i - 1] == val) {
                _buf.removeAt(i - 1);
                changed = true;
                i--;
            }
        }

        return changed;
    }

    // MARK: Capacity

    void ensure(usize cap) { _buf.ensure(cap); }

    void trunc(usize len) { _buf.trunc(len); }

    void resize(usize len, T fill = {}) { _buf.resize(len, fill); }

    void fit() { _buf.fit(); }

    void clear() { _buf.trunc(0); }

    usize cap() const { return _buf.cap(); }

    // MARK: Random Access

    void insert(usize index, T const& value) { _buf.insert(index, T(value)); }

    void insert(usize index, T&& value)
    {
        _buf.insert(index, Std::move(value));
    }

    void insertMany(usize index, Sliceable<T> auto const& other)
    {
        for (auto& v : other) {
            insert(index++, v);
        }
    }

    void replace(usize index, T const& value) { _buf[index] = T(value); }

    void replace(usize index, T&& value) { _buf[index] = Std::move(value); }

    T removeAt(usize index) { return _buf.removeAt(index); }

    void removeRange(usize index, usize count)
    {
        _buf.removeRange(index, count);
    }

    // MARK: Front Access

    void pushFront(T const& value) { _buf.insert(0, T(value)); }

    void pushFront(T&& value) { _buf.insert(0, Std::move(value)); }

    void pushFront(Sliceable<T> auto& other)
    {
        for (auto& v : other)
            pushFront(v);
    }

    template <typename... Args>
    T& emplaceFront(Args&&... args)
    {
        return _buf.emplace(0, Std::forward<Args>(args)...);
    }

    T popFront() { return _buf.removeAt(0); }

    // MARK: Back Access

    void pushBack(T const& value) { insert(len(), value); }

    void pushBack(T&& value) { insert(len(), Std::move(value)); }

    void pushBack(Sliceable<T> auto& other)
    {
        for (auto& v : other)
            pushBack(v);
    }

    template <typename... Args>
    T& emplaceBack(Args&&... args)
    {
        return _buf.emplace(len(), Std::forward<Args>(args)...);
    }

    T popBack() { return removeAt(len() - 1); }

    void inverse()
    {
        for (usize i = 0; i < len() / 2; i++) {
            T tmp               = _buf[i];
            _buf[i]             = _buf[len() - i - 1];
            _buf[len() - i - 1] = tmp;
        }
    }

    // MARK: MutSliceable

    constexpr usize len() const { return _buf.len(); }

    constexpr T* buf() { return _buf.buf(); }

    constexpr T const* buf() const { return _buf.buf(); }

    constexpr T& operator[](usize i)
    {
        if (i >= len()) [[unlikely]]
            Std::panic("index out of bounds");

        return _buf[i];
    }

    constexpr T const& operator[](usize i) const
    {
        if (i >= len()) [[unlikely]]
            Std::panic("index out of bounds");

        return _buf[i];
    }

    constexpr explicit operator bool() const { return len(); }
};

template <typename T>
using Vec = _Vec<Buf<T>>;

template <typename T, usize N>
using StackVec = _Vec<Buf<T[N]>>;