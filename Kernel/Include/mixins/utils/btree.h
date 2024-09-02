#include <mixins/std/c++types.h>
/**
 * @brief BTree is a data structure used to store data in a self-balancing tree.
 *
 * @tparam K Key
 * @tparam V Value
 * @tparam Count Number of children a node can have
 */
template <typename K, typename V, usize Count>
class BTree
{
    static constexpr usize MaxNodeCounts = Count;
    static constexpr usize MinNodeCounts = Count / 2;

public:
    BTree();

private:
    struct Node
    {
        K     _keys[MaxNodeCounts];
        V     _values[MaxNodeCounts];
        Node* _children[MaxNodeCounts + 1];
        usize _size;
    };
    Node* m_root;
};