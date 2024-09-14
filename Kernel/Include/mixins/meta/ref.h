#pragma once

#include <mixins/std/c++types.h>

template <typename T>
class RefPtr
{
public:
    RefPtr()
        : _ptr(nullptr)
        , _refCount(new usize(1))
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