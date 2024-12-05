#pragma once

#include <mixins/concurrent/atomic.h>
#include <mixins/std/assert.h>
#include <mixins/std/c++types.h>
#include <mixins/std/utility.h>

template <typename T>
class RefPtr
{
public:
    RefPtr()
        : _ptr(nullptr)
        , _refCount(nullptr)
    {
    }
    RefPtr(T* ptr)
        : _ptr(ptr)
        , _refCount(new usize(1))
    {
    }
    RefPtr(RefPtr const& other)
        : _ptr(other._ptr)
        , _refCount(other._refCount)
    {
        ++*_refCount;
    }
    RefPtr(RefPtr&& other)
        : _ptr(other._ptr)
        , _refCount(other._refCount)
    {
        other._ptr      = nullptr;
        other._refCount = nullptr;
    }
    ~RefPtr()
    {
        if (--(*_refCount) == 0) {
            delete _ptr;
            delete _refCount;
        }
    }

    RefPtr& operator=(const RefPtr& other)
    {
        if (this != &other) {
            if (--*_refCount == 0) {
                delete _ptr;
                delete _refCount;
            }
            _ptr      = other._ptr;
            _refCount = other._refCount;
            ++*_refCount;
        }
        return *this;
    }
    RefPtr& operator=(RefPtr&& other)
    {
        if (this != &other) {
            if (--*_refCount == 0) {
                delete _ptr;
                delete _refCount;
            }
            _ptr            = other._ptr;
            _refCount       = other._refCount;
            other._ptr      = nullptr;
            other._refCount = nullptr;
        }
        return *this;
    }

    T& operator*() { return *_ptr; }
    T* operator->() { return _ptr; }

    const T& operator*() const { return *_ptr; }
    const T* operator->() const { return _ptr; }

    usize refCount() const { return *_refCount; }

private:
    T*     _ptr;
    usize* _refCount;
};

template <typename T>
class WeakRef
{
public:
    WeakRef()
        : _ptr(nullptr)
        , _refCount(nullptr)
    {
    }
    WeakRef(const RefPtr<T>& ref)
        : _ptr(ref._ptr)
        , _refCount(ref._refCount)
    {
    }
    WeakRef(const WeakRef& other)
        : _ptr(other._ptr)
        , _refCount(other._refCount)
    {
    }
    WeakRef(WeakRef&& other)
        : _ptr(other._ptr)
        , _refCount(other._refCount)
    {
        other._ptr      = nullptr;
        other._refCount = nullptr;
    }
    ~WeakRef() {}

    WeakRef& operator=(const WeakRef& other)
    {
        if (this != &other) {
            _ptr      = other._ptr;
            _refCount = other._refCount;
        }
        return *this;
    }
    WeakRef& operator=(WeakRef&& other)
    {
        if (this != &other) {
            _ptr            = other._ptr;
            _refCount       = other._refCount;
            other._ptr      = nullptr;
            other._refCount = nullptr;
        }
        return *this;
    }

    RefPtr<T> lock() const { return RefPtr<T>(_ptr); }

private:
    T*     _ptr;
    usize* _refCount;
};

namespace Qk {
    template <typename T>
    class NonnullRefPtr
    {
    public:
        always_inline constexpr NonnullRefPtr() = delete;
        always_inline constexpr NonnullRefPtr(T& ptr)
            : m_ptr(&ptr)
        {
        }
        always_inline constexpr NonnullRefPtr(T* ptr)
            : m_ptr(ptr)
        {
            assert(m_ptr != nullptr);
        }
        always_inline constexpr NonnullRefPtr(NonnullRefPtr const& other)
            : m_ptr(other.m_ptr)
        {
        }
        always_inline constexpr NonnullRefPtr(NonnullRefPtr&& other)
            : m_ptr(other.m_ptr)
        {
            Std::exchange(other.m_ptr, nullptr);
        }

        template <typename U>
            requires(Std::isConvertible<U*, T*>)
        always_inline constexpr NonnullRefPtr(U const& other)
            : m_ptr(const_cast<T*>(static_cast<T const*>(&other)))
        {
        }

        template <typename U>
            requires(Std::isConvertible<U*, T*>)
        always_inline constexpr NonnullRefPtr(NonnullRefPtr<U> const& other)
            : m_ptr(const_cast<T*>(static_cast<T const*>(other.get()))){};

        NonnullRefPtr& operator=(NonnullRefPtr const& other)
        {
            m_ptr = other.m_ptr;
            return *this;
        }

        NonnullRefPtr& operator=(NonnullRefPtr&& other)
        {
            m_ptr = other.m_ptr;
            Std::exchange(other.m_ptr, nullptr);
            return *this;
        }

        always_inline constexpr T& operator*() const { return *nonnull(); }

        always_inline constexpr T* operator->() const { return nonnull(); }

        always_inline constexpr T* get() const { return nonnull(); }

        always_inline constexpr T* release()
        {
            T* ptr = nonnull();
            Std::exchange(m_ptr, nullptr);
            return ptr;
        }

        always_inline constexpr bool operator==(
            NonnullRefPtr const& other) const
        {
            return m_ptr == other.m_ptr;
        }

        always_inline constexpr bool operator!=(
            NonnullRefPtr const& other) const
        {
            return m_ptr != other.m_ptr;
        }

        always_inline constexpr bool operator==(T const* other) const
        {
            return m_ptr == other;
        }

        always_inline constexpr bool operator!=(T const* other) const
        {
            return m_ptr != other;
        }

        always_inline constexpr T* nonnull() const
        {
            assert(m_ptr != nullptr, "Nonnull reference is null");
            return m_ptr;
        }

        operator bool() const  = delete;
        bool operator!() const = delete;

    private:
        T* m_ptr{ nullptr };
    };

    template <typename T>
        requires(Destructible<T>)
    class RefCounted
    {
        using RefCountType = u32;

    private:
    };

    template <typename T>
        requires(Destructible<T>)
    class AtomicRefCounted
    {
    public:
        using RefCountType = u32;

        always_inline NonnullRefPtr<T> ref() const
        {
            _refCount.Inc(MemoryOrderRelaxed);
            return NonnullRefPtr<T>(static_cast<T&>(*this));
        }

        always_inline void deref(NonnullRefPtr<T>& ref) const
        {
            if (ref.get() != this) {
                return;
            }

            if (_refCount.FetchSub(1, MemoryOrderRelease) == 0) {
                delete this;
            }
        }

    private:
        mutable Atomic<RefCountType> _refCount{ 1 };
    };

    template <typename T>
    class OwnedRefPtr
    {};
}