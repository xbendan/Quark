#include <mixins/std/c++types.h>
#include <mixins/utils/collections.h>

template <typename TSource>
class Stack
    : public ICollection<TSource>
    , public IReadOnlyCollection<TSource>
{
public:
    TSource const& peek() {}

    TSource& pop() {}

    TSource& push(TSource const& item) {}

private:
    TSource* _data;
    u32      _count;
    u32      _depth;
};