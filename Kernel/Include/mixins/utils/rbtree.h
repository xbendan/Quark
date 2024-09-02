template <typename K, typename V>
class RbTree
{
private:
    enum class NodeType
    {
        RED,
        BLACK
    };

    struct TreeNode
    {
        K         _key;
        V         _value;
        NodeType  _type;
        TreeNode* _left;
        TreeNode* _right;
        TreeNode* _parent;
    };
};