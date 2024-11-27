#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/list.h>

namespace Qk {
    template <typename TSource>
    class Singleton : public List<TSource>
    {
    public:
        Singleton()  = default;
        ~Singleton() = default;

        Optional<TSource> Select(TSource* selected)
        {
            if (List<TSource>::Contains(selected)) {
                return m_selected = selected;
            } else {
                return Empty();
            }
        }

        Optional<TSource> GetOrFirst()
        {
            if (!m_selected) {
                m_selected = List<TSource>::FindFirst();
            }

            return m_selected.Get();
        }

        TSource& operator()()
        {
            if (!m_selected) {
                m_selected = List<TSource>::FindFirst();
            }

            return m_selected.Get();
        }

    private:
        Optional<TSource&> m_selected = Empty();
    };
}