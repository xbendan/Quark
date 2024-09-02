#include <mixins/std/concepts.h>

template <typename TIterator, typename TSource>
concept Iterator = requires(TIterator const& it) {
    { it.operator--() };
    { it.operator++() };
    { it.operator()() } -> SameAs<TSource&>;
};

template <typename TIterable, typename TSource>
concept Iterable = requires(TIterable const& it) {
    { it.iter() } -> Iterator<TSource>;
};