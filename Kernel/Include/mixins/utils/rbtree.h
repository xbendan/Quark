#include <mixins/math/compute.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/hash.h>

// Reference: https://github.com/torvalds/linux/blob/master/lib/rbtree.c

namespace Qk {
    template <typename TKey, typename TValue>
    class RbTree : public IKeyedAccess<TKey, TValue>
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
            TKey      _key;
            TValue    _value;
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

        bool ContainsKey(TKey const& key) const {}

        bool ContainsValue(TValue const& value) const {}

        TValue& Put(TKey const& key, TValue const& value) {}

        TValue& PutIfAbsent(TKey const& key, TValue const& value) {}

        bool Remove(TKey const& key) {}

        bool Remove(TKey const& key, TValue const& value) {}

        void Clear() {}

        bool IsEmpty() const {}

        bool Replace(TKey const& key, TValue const& value) {}

        bool Replace(TKey const&   key,
                     TValue const& oldValue,
                     TValue const& newValue)
        {
        }

        usize Count() const {}

        void ForEach(Func<void(TKey const&, TValue const&)> action) const {}

        virtual TValue& operator[](TKey const& key) override {}

        virtual TValue const& operator[](TKey const& key) const override {}

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

        Hash<TKey> m_hash;
        TreeNode*  m_root;
    };
}