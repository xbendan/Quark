#include <mixins/math/compute.h>
#include <mixins/utils/hash.h>

// Reference: https://github.com/torvalds/linux/blob/master/lib/rbtree.c

template <typename K, typename V>
class RbTree
{
    enum class NodeType
    {
        RED   = 0,
        BLACK = 1,
    };

    friend NodeType operator!(NodeType type)
    {
        return (type == NodeType::RED) ? NodeType::BLACK : NodeType::RED;
    }

    struct TreeNode
    {
        K         _key;
        V         _value;
        NodeType  _type;
        TreeNode* _left;
        TreeNode* _right;
        TreeNode* _parent;
    };

public:
    RbTree()
        : m_root(nullptr)
    {
    }

    ~RbTree()
    {
        // Clear nodes
    }

private:
    void SetNodeTypeRecursively(TreeNode* node, NodeType type)
    {
        node->_type = type;

        if (node->_left) {
            SetNodeTypeRecursively(node->_left, !type);
        }
        if (node->_right) {
            SetNodeTypeRecursively(node->_right, !type);
        }
    }

    Hash<K>   m_hash;
    TreeNode* m_root;
};