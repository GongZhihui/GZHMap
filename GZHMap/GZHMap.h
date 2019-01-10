#ifndef SHAMAP_H
#define SHAMAP_H

#include <vector>
#include <string>
#include <memory>
#include "util.h"

class NodeID
{
public:
    NodeID() = default;
    NodeID(const uint32 &id, int depth)
        : mID(id)
        , mDepth(depth)
    {
    }
    int depth() const;
    const uint32 & id() const;

    std::string to_string() const;
private:
    uint32 mID = {};
    int mDepth = 0;
};

class TreeNode
{
public:
    using pointer = std::shared_ptr<TreeNode>;

    TreeNode() = default;
    TreeNode(const TreeNode &) = delete;
    TreeNode& operator=(const TreeNode &) = delete;
    virtual ~TreeNode() = default;

public:
    virtual int depth() const = 0;
    virtual const uint32& key() const = 0;
    virtual std::string to_string() const = 0;
    bool isLeaf() const;

};

class LeafNode;

class InnerNode : public TreeNode
{
public:
    using Children = std::array<TreeNode::pointer, 16>;

    InnerNode() = default;

    int depth() const override;
    const uint32& key() const override;
    std::string to_string() const override;

    void setChild(int branch, const TreeNode::pointer &child);
    TreeNode::pointer getChild(int branch) const;

    void setCommon(int depth, const uint32 &key);
    const uint32& getCommon() const;

    bool isBranch(int n) const;
    void setBranch(int n);
    int selectNextBranch(const uint32 &key) const;
private:

private:
    int mDepth = 0;

    // 从右往左  如0001 是第一个分支，0010 第二个分支 0100 第三个分支
    unsigned int branch = 0;
    uint32 common = {};
    Children children;

};

class Item
{
public:
    using pointer = std::shared_ptr<const Item>;

public:
    Item(const uint32 &tag, int value);
    const uint32 &key() const;
    std::string to_string() const;
private:
    uint32 mTag = {};
    int mValue;
};

class LeafNode : public TreeNode
{
public:
    LeafNode() = default;
    LeafNode(const Item &item);
    
    const Item::pointer &item() const;
    void setItem(const Item &item);

    std::string to_string() const override;
    int depth() const override;
    const uint32& key() const override;
private:
    Item::pointer mItem;
};

class GZHMap
{
public:
    using NodeStack = std::vector<
        std::pair<TreeNode::pointer, NodeID>>;

public:
    GZHMap();
    bool insert(const Item& item);
    const Item::pointer & peekItem(const uint32 &key);

private:
    TreeNode * walkTree(const uint32 &key, NodeStack &stack);
private:
    TreeNode::pointer mRoot;
};

//-----------------------------------
inline bool TreeNode::isLeaf() const
{
    return dynamic_cast<const LeafNode*>(this) != nullptr;
}

#endif // !SHAMAP_H