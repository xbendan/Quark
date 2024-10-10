#include <mixins/concurrent/spinlock.h>
#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array_list.h>

template <typename... Args>
class EventArgs
{};

template <typename... Args>
class Event
{
public:
    Event()  = default;
    ~Event() = default;

    void operator+=(void (*handler)(EventArgs<Args...>* args))
    {
        m_handlers.Add(handler);
    }

    void operator+=(Func<void(EventArgs<Args...>* args)> handler)
    {
        m_handlers.Add(handler);
    }

    void operator-=(Func<void(EventArgs<Args...>* args)> handler)
    {
        m_handlers.Remove(handler);
    }

    void operator()(Args... args)
    {
        for (usize i = 0; i < m_handlers.Count(); i++) {
            m_handlers[i](args...);
        }
    }

    bool operator==(nullptr_t) { return m_handlers.IsEmpty(); }

    bool operator!=(nullptr_t) { return !m_handlers.IsEmpty(); }

private:
    Spinlock                                        m_lock;
    ArrayList<Func<void(EventArgs<Args...>* args)>> m_handlers;
};

template <typename TSelectionType>
class SelectionChangedEventArgs : public EventArgs<TSelectionType>
{};