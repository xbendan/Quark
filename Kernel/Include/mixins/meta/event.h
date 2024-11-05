#pragma once

#include <mixins/concurrent/spinlock.h>
#include <mixins/meta/func.h>
#include <mixins/meta/inert.h>
#include <mixins/std/c++types.h>
#include <mixins/str/string_view.h>
#include <mixins/utils/list.h>

namespace Qk {
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

        void Invoke(Args... args) {}

        void operator()(Args... args)
        {
            for (auto& handler : m_handlers) {
                handler(args...);
            }
        }

        // bool operator==(nullptr_t) { return m_handlers.IsEmpty(); }

        // bool operator!=(nullptr_t) { return !m_handlers.IsEmpty(); }

        bool operator==(Event const& other) const
        {
            return m_handlers == other.m_handlers;
        }

    private:
        lock_t                  m_lock{ 0 };
        Qk::StringView          m_name;
        List<void (*)(Args...)> m_handlers;
    };
}