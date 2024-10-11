#include <mixins/concurrent/spinlock.h>
#include <mixins/meta/func.h>
#include <mixins/meta/inert.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array_list.h>

template <typename... Args>
class Event
{
public:
    Event()
        : m_handlers(1)
    {
    }
    ~Event() = default;

    bool operator+=(void (*handler)(Args... args))
    {
        if (handler == nullptr || m_handlers.Contains(handler)) {
            return false;
        }

        m_handlers.Add(handler);
        return true;
    }

    void operator-=(void (*handler)(Args... args))
    {
        m_handlers.Remove(handler);
    }

    void operator()(Args... args)
    {
        m_handlers.ForEach([&](void (*handler)(Args...)) { handler(args...); });
    }

    // bool operator==(nullptr_t) { return m_handlers.IsEmpty(); }

    // bool operator!=(nullptr_t) { return !m_handlers.IsEmpty(); }

    bool operator==(Event const& other) const
    {
        return m_handlers == other.m_handlers;
    }

private:
    Spinlock                     m_lock;
    ArrayList<void (*)(Args...)> m_handlers{ 1 };
};