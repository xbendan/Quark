#include <mixins/math/compute.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/hash.h>

// Reference: https://github.com/torvalds/linux/blob/master/lib/rbtree.c
// Reference: https://github.com/Martiniele/Rbtree/blob/master/RBtree.h

namespace Qk {
    template <typename TKey, typename TValue>
    class RbTree
    {
        using KeyType   = Std::Conditional<Comparable<TKey>, TKey, u64>;
        using ValueType = TValue;

        enum class NodeType
        {
            RED   = 0,
            BLACK = 1
        };

        friend NodeType operator!(NodeType type)
        {
            return (type == NodeType::RED) ? NodeType::BLACK : NodeType::RED;
        }

        enum class Direction
        {
            LEFT,
            RIGHT
        };

        struct TreeNode
        {
            KeyType   _key;
            ValueType _value;
            NodeType  _type;
            TreeNode* _left;
            TreeNode* _right;
            TreeNode* _parent;

            bool IsRed() const { return _type == NodeType::RED; }
            bool IsBlack() const { return _type == NodeType::BLACK; }
            bool IsRightChild() const
            {
                return _parent != nullptr and _parent->_right == this;
            }
            bool IsLeftChild() const
            {
                return _parent != nullptr and _parent->_left == this;
            }
        };

    public:
        RbTree() = default;

        ~RbTree()
        {
            // Clear nodes
        }

        TreeNode* Query(TKey const& key, TreeNode* n) const
        {
            KeyType k = GenerateKey(key);
            while ((n != nullptr) && (k != n->_key)) {
                n = (k < n->_key) ? n->_left : n->_right;
            }
            return n;
        }

        TreeNode* Lowest(TreeNode* n) const
        {
            while (n->_left != nullptr) {
                n = n->_left;
            }
            return n;
        }

        TreeNode* Highest(TreeNode* n) const
        {
            while (n->_right != nullptr) {
                n = n->_right;
            }
            return n;
        }

        TreeNode* Predecessor(TreeNode* n) const
        {
            if (n->_left != nullptr) {
                return Highest(n->_left);
            }

            TreeNode* p = n->_parent;
            while ((p != nullptr) && (n == p->_left)) {
                n = p;
                p = p->_parent;
            }
            return p;
        }

        TreeNode* Successor(TreeNode* n) const
        {
            if (n->_right != nullptr) {
                return Lowest(n->_right);
            }

            TreeNode* p = n->_parent;
            while ((p != nullptr) && (n == p->_right)) {
                n = p;
                p = p->_parent;
            }
            return p;
        }

        TreeNode const* Rotate(TreeNode* n, Direction di)
        {
            // clang-format off
            /*
            
                 |                       |
                 N                       S
                / \     l-rotate(N)     / \
               L   S    ==========>    N   R
                  / \                 / \
                 M   R               L   M
             
                 |                       |
                 S                       N
                / \     r-rotate(N)     / \
               N   R    ==========>    L   S
              / \                         / \
             L   M                       M   R

            */
            // clang-format on
            TreeNode* parent = n->_parent;
            TreeNode* child  = (di == Direction::LEFT) ? n->_right : n->_left;
            if (child == nullptr) {
                return n;
            }

            n->_parent = child;
            if (di == Direction::LEFT) {
                n->_right    = child->_left;
                child->_left = n;
                if (n->_right != nullptr) {
                    n->_right->_parent = n;
                }
            } else {
                n->_left      = child->_right;
                child->_right = n;
                if (n->_left != nullptr) {
                    n->_left->_parent = n;
                }
            }
            child->_parent = parent;

            if (parent != nullptr) {
                if (parent->_left == n)
                    parent->_left = child;
                else
                    parent->_right = child;

            } else {
                m_root = child;
            }

            return child;
        }

        bool ContainsKey(TKey const& key) const
        {
            return Query(key) != nullptr;
        }

        bool ContainsValue(TValue const& value, TreeNode* node) const
        {
            if (node == nullptr) {
                return false;
            }

            if (node->_value == value) {
                return true;
            }

            return ContainsValue(value, node->_left) ||
                   ContainsValue(value, node->_right);
        }

        bool ContainsValue(TValue const& value) const
        {
            return ContainsValue(value, m_root);
        }

        TValue& Put(TKey const& key, TValue const& value)
        {
            auto* node = new TreeNode{
                Std::move(key), Std::move(value), NodeType::BLACK,
                nullptr,        nullptr,          nullptr,
            };
            if (m_root == nullptr) {
                m_count++;
                return (m_root = node)->_value;
            }

            KeyType   k = GenerateKey(key);
            TreeNode* n = m_root;
            while (n != nullptr) {
                if (k < n->_key) {
                    if (n->_left == nullptr) {
                        n->_left      = node;
                        node->_parent = n;
                        break;
                    }
                    n = n->_left;
                } else if (k > n->_key) {
                    if (n->_right == nullptr) {
                        n->_right     = node;
                        node->_parent = n;
                        break;
                    }
                    n = n->_right;
                } else {
                    /* Replace the origin value */
                    n->_value = value;
                    return n->_value;
                }
            }
            m_count++;
            node->_type = NodeType::RED;
            RebalanceOfAdding(m_root, node);

            return node->_value;
        }

        TValue& PutIfAbsent(TKey const& key, TValue const& value)
        {
            auto* node = new TreeNode{
                Std::move(key), Std::move(value), NodeType::BLACK,
                nullptr,        nullptr,          nullptr,
            };
            if (m_root == nullptr) {
                m_count++;
                return (m_root = node)->_value;
            }

            KeyType   k = GenerateKey(key);
            TreeNode* n = m_root;
            while (n != nullptr) {
                if (k < n->_key) {
                    if (n->_left == nullptr) {
                        n->_left      = node;
                        node->_parent = n;
                        break;
                    }
                    n = n->_left;
                } else if (k > n->_key) {
                    if (n->_right == nullptr) {
                        n->_right     = node;
                        node->_parent = n;
                        break;
                    }
                    n = n->_right;
                } else {
                    return n->_value;
                }
            }
            m_count++;
            node->_type = NodeType::RED;
            RebalanceOfAdding(m_root, node);

            return node->_value;
        }

        bool Remove(TKey const& key)
        {
            TreeNode* node = Query(key);
            if (node == nullptr) {
                return false;
            }

            TreeNode *child, *parent;
            NodeType  type;
            if ((node->_left != nullptr) && (node->_right != nullptr)) {
                TreeNode* successor = node->_right;
                while (successor->_left != nullptr) {
                    successor = successor->_left;
                }

                if (successor->_parent != nullptr) {
                    if (successor->IsLeftChild())
                        successor->_parent->_left = successor->_right;
                    else
                        successor->_parent->_right = successor->_right;
                } else
                    m_root = successor;

                child  = successor->_right;
                parent = successor->_parent;
                type   = successor->_type;

                if (parent == node) {
                    parent = successor;
                } else {
                    if (child != nullptr)
                        child->_parent = parent;
                    parent->_left = child;

                    successor->_right     = node->_right;
                    node->_right->_parent = successor;
                }

                successor->_parent   = node->_parent;
                successor->_type     = node->_type;
                successor->_left     = node->_left;
                node->_left->_parent = successor;
            } else {
                if (node->_left != nullptr)
                    child = node->_left;
                else
                    child = node->_right;

                parent = node->_parent;
                type   = node->_type;

                if (child != nullptr)
                    child->_parent = parent;

                if (parent != nullptr) {
                    if (node->IsLeftChild())
                        parent->_left = child;
                    else
                        parent->_right = child;
                } else
                    m_root = child;
            }

            if (type == NodeType::BLACK)
                RebalanceOfRemoving(m_root, child, parent);
            delete node;
        }

        bool Remove(TKey const& key, TValue const& value)
        {
            TreeNode* node = Query(key);
            if (node == nullptr) {
                return false;
            }

            if (node->_value != value) {
                return false;
            }

            if (node->_left != nullptr && node->_right != nullptr) {
                TreeNode* successor = node->_right;
                while (successor->_left != nullptr) {
                    successor = successor->_left;
                }
                node->_key   = successor->_key;
                node->_value = successor->_value;
                node         = successor;
            } else {
                TreeNode* child =
                    (node->_left != nullptr) ? node->_left : node->_right;
                if (node->_type == NodeType::BLACK) {
                    node->_type = child->_type;
                    DeleteFixup(node);
                }
                ReplaceNode(node, child);
            }
        }

        void Clear()
        {
            // Clear nodes
        }

        bool Replace(TKey const& key, TValue const& value)
        {
            auto* node = Query(key);
            if (node == nullptr) {
                return false;
            }

            node->_value = Std::move(value);
            return true;
        }

        bool Replace(TKey const&   key,
                     TValue const& oldValue,
                     TValue const& newValue)
        {
            auto* node = Query(key);
            if (node == nullptr or node->_value != oldValue) {
                return false;
            }

            node->_value = Std::move(newValue);
            return true;
        }

        usize Count() const { return m_count; }

        bool IsEmpty() const { return m_count == 0; }

        void ForEach(Func<void(TKey const&, TValue const&)> action) const {}

        virtual Optional<TValue&> operator[](TKey const& key)
        {
            TreeNode* node = Query(key, m_root);
            if (node == nullptr) {
                return Empty{};
            }
            return node->_value;
        }

        // virtual Opt<TValue const&> operator[](TKey const& key) const
        // {
        //     TreeNode* node = Query(key, m_root);
        //     if (node == nullptr) {
        //         return Empty{};
        //     }
        //     return node->_value;
        // }

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

        KeyType GenerateKey(TKey const& key) const
        {
            if constexpr (Comparable<TKey>) {
                return key;
            }
            return m_hash(key);
        }

        void RebalanceOfAdding(TreeNode* root, TreeNode* n)
        {
            assert(root != nullptr and n != nullptr and
                   n->_type == NodeType::RED);
            TreeNode *par, *grandpar;
            while ((par = n->_parent) and par->IsRed()) {
                if (par == grandpar->_left) {
                    TreeNode* uncle = grandpar->_right;
                    if (uncle and uncle->IsRed()) {
                        uncle->_type    = NodeType::BLACK;
                        par->_type      = NodeType::BLACK;
                        grandpar->_type = NodeType::RED;
                        n               = grandpar;
                        continue;
                    }

                    if (n == par->_right) /* Case 2: uncle is black and n is
                                             right child */
                    {
                        Rotate(par, Direction::LEFT);
                        Std::exchange(n, par);
                    } else /* Case 3: uncle is black and n is left child */ {
                        par->_type      = NodeType::BLACK;
                        grandpar->_type = NodeType::RED;
                        Rotate(grandpar, Direction::RIGHT);
                    }
                } else {
                    TreeNode* uncle = grandpar->_left;
                    if (uncle and uncle->IsRed()) {
                        uncle->_type    = NodeType::BLACK;
                        par->_type      = NodeType::BLACK;
                        grandpar->_type = NodeType::RED;
                        n               = grandpar;
                        continue;
                    }

                    if (n == par->_left) /* Case 2: uncle is black and n is
                                            left child */
                    {
                        Rotate(par, Direction::RIGHT);
                        Std::exchange(n, par);
                    } else /* Case 3:
                            uncle is black and n is right child */
                    {
                        par->_type      = NodeType::BLACK;
                        grandpar->_type = NodeType::RED;
                        Rotate(grandpar, Direction::LEFT);
                    }
                }
            }
            root->_type = NodeType::BLACK;
        }

        void RebalanceOfRemoving(TreeNode* root, TreeNode* n, TreeNode* par)
        {
            TreeNode* other = nullptr;

            while ((!n || n->IsBlack()) && n != root) {
                if (par->_left == n) {
                    other = par->_right;
                    if (other->IsRed()) {
                        other->_type = NodeType::BLACK;
                        par->_type   = NodeType::RED;
                        Rotate(par, Direction::LEFT);
                        other = par->_right;
                    }
                    if ((!other->_left || other->_left->IsBlack()) &&
                        (!other->_right || other->_right->IsBlack())) {
                        other->_type = NodeType::RED;
                        n            = par;
                        par          = n->_parent;
                    } else {
                        if (!other->_right || other->_right->IsBlack()) {
                            other->_left->_type = NodeType::BLACK;
                            other->_type        = NodeType::RED;
                            Rotate(other, Direction::RIGHT);
                            other = par->_right;
                        }
                        other->_type         = par->_type;
                        par->_type           = NodeType::BLACK;
                        other->_right->_type = NodeType::BLACK;
                        Rotate(par, Direction::LEFT);
                        n = root;
                        break;
                    }
                } else {
                    other = par->_left;
                    if (other->IsRed()) {
                        other->_type = NodeType::BLACK;
                        par->_type   = NodeType::RED;
                        Rotate(par, Direction::RIGHT);
                        other = par->_left;
                    }
                    if ((!other->_left || other->_left->IsBlack()) &&
                        (!other->_right || other->_right->IsBlack())) {
                        other->_type = NodeType::RED;
                        n            = par;
                        par          = n->_parent;
                    } else {
                        if (!other->_left || other->_left->IsBlack()) {
                            other->_right->_type = NodeType::BLACK;
                            other->_type         = NodeType::RED;
                            Rotate(other, Direction::LEFT);
                            other = par->_left;
                        }
                        other->_type        = par->_type;
                        par->_type          = NodeType::BLACK;
                        other->_left->_type = NodeType::BLACK;
                        Rotate(par, Direction::RIGHT);
                        n = root;
                        break;
                    }
                }
            }
            if (n)
                n->_type = NodeType::BLACK;
        }

        Hash<TKey> m_hash;
        TreeNode*  m_root;
        usize      m_count;
    };
}