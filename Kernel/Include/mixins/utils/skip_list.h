#include <mixins/std/c++types.h>

template <typename TSource, usize TLen = 2, usize K = 8>
class SkipList
{
public:
    SkipList() = default;
    ~SkipList()
    {
        for (int i = 1; i < K; i++) {
            NodeIndex* index = _index[i];
            while (index) {
                NodeIndex* next = index->next;
                delete index;
                index = next;
            }
        }

        Node* node = _head;
        while (node) {
            Node* next = node->next;
            delete node;
            node = next;
        }
    }

private:
    struct Node
    {
        TSource data;
        Node*   next;
    };
    struct NodeIndex
    {
        NodeIndex* next;
        union
        {
            Node*      node;
            NodeIndex* down;
        };
    };

    NodeIndex* _index[K];
    Node*      _head;
};