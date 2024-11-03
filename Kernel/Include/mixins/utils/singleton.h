#include <mixins/std/c++types.h>
#include <mixins/std/concepts.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/linked_list.h>

template <typename TSource, template <typename> typename TImpl = LinkedList>
    requires Std::isDerived<ICollection<TSource>, TImpl<TSource>>
class Singleton : public TImpl<TSource>
{
public:
    Singleton()  = default;
    ~Singleton() = default;

    Opt<TSource> Select(TSource* selected)
    {
        if (TImpl<TSource>::Contains(selected)) {
            return m_selected = selected;
        } else {
            return Empty();
        }
    }

    Opt<TSource> GetOrFirst()
    {
        if (!m_selected) {
            m_selected = TImpl<TSource>::FindFirst();
        }

        return m_selected.Get();
    }

    TSource* operator()() const { return GetOrFirst(); }

private:
    Opt<TSource&> m_selected = Empty();
};