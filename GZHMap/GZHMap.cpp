#include "GZHMap.h"
#include "util.h"

#include <assert.h>
#include <iostream>

//----------------NodeID------------------
int NodeID::depth() const
{
    return mDepth;
}

const uint32 & NodeID::id() const
{
    return mID;
}

std::string NodeID::to_string() const
{
    std::string ret;
    ret = "id: " + string2hex(mID.data(), mID.size());
    ret += " | ";
    ret += "depth: ";
    ret += std::to_string(mDepth);
    return ret;
}

//---------------InnerNode----------------
std::string InnerNode::to_string() const
{
    return "inner node";
}

int InnerNode::depth() const
{
    return mDepth;
}

const uint32& InnerNode::key() const
{
    return common;
}

bool InnerNode::isBranch(int n) const
{
    return branch & (1 << n);
}

void InnerNode::setBranch(int n)
{
    branch |= (1 << n);
}

int InnerNode::selectNextBranch(const uint32 &key) const
{
    //std::cout << "InnerNode depth" << mDepth << std::endl;
    assert((mDepth >= 0) && (mDepth < 8));
    auto n = mDepth;
    auto c = key[n / 2];

    auto p = n % 2;
    if (p == 0)
    {
        c &= 0xF0;
        c >>= 4;
    }
    else
    {
        c &= 0x0F;
    }
    return c;
}

void InnerNode::setChild(int branch, const TreeNode::pointer &child)
{
    assert(branch <= 15);
    setBranch(branch);
    children[branch] = child;
}

TreeNode::pointer InnerNode::getChild(int branch) const
{
    return children[branch];
}

void InnerNode::setCommon(int depth, const uint32 &key)
{
    mDepth = depth;
    auto temp = key;
    depth--;
    unsigned char t[4] = { 0 };

    if (depth % 2 == 0)
    {
        t[depth / 2] = 0xF0;
    }
    else
    {
        t[depth / 2] = 0xFF;
    }
    for (int i = 0; i < depth / 2; i++)
    {
        t[i] = 0xFF;
    }
    int i = 0;
    for (auto &j : temp)
    {
        j &= t[i];
        ++i;
    }
    common = std::move(temp);

}
const uint32& InnerNode::getCommon() const
{
    return common;
}

//------------------Item---------------------
Item::Item(const uint32 &tag, int value)
    : mTag(tag)
    , mValue(value)
{
}

std::string Item::to_string() const
{
    return "tag: " + string2hex(mTag.data(), mTag.size())
        + " | " + "value: " + std::to_string(mValue);
}

const uint32 &Item::key() const
{
    return mTag;
}
//------------------TreeNode-----------------------
// TreeNode::TreeNode()
// {    
// }
//----------------LeafNode------------------
LeafNode::LeafNode(const Item &item)
    : TreeNode()
    , mItem(std::make_shared<const Item>(item))
{
}

const Item::pointer &LeafNode::item() const
{
    return mItem;
}

void LeafNode::setItem(const Item &item)
{
    mItem = std::make_shared<const Item>(item);
}

std::string LeafNode::to_string() const
{
    return "LeafNode: " + mItem->to_string();
}

int LeafNode::depth() const
{
    return 8;
}

const uint32& LeafNode::key() const
{
    return mItem->key();
}

//-------------------GZHMap---------------------
GZHMap::GZHMap()
    : mRoot(std::make_shared<InnerNode>())
{
}

TreeNode *GZHMap::walkTree(const uint32 &key, NodeStack &stack)
{
    auto root = mRoot;
    stack.push_back({ root,{ root->key(), root->depth() } });
    auto innerNode = std::static_pointer_cast<InnerNode>(root);
    int flag = 0;
    while (flag < 7)
    {
        int branch = innerNode->selectNextBranch(key);
        auto child = std::static_pointer_cast<InnerNode>(innerNode->getChild(branch));
        if (child == nullptr)
        {
            child = std::make_shared<InnerNode>();
            int currentDepth = innerNode->depth();
            child->setCommon(++currentDepth, key);
            innerNode->setChild(branch, child);
        }
        stack.push_back({ child,{ child->key(), child->depth() } });
        innerNode = child;
        ++flag;
    }

    int branch = innerNode->selectNextBranch(key);
    auto child = std::static_pointer_cast<LeafNode>(innerNode->getChild(branch));
    if (child == nullptr)
    {
        child = std::make_shared<LeafNode>();
    }
    innerNode->setChild(branch, child);
    stack.push_back({ child, {}});
    return child.get();

}

bool GZHMap::insert(const Item& item)
{
    auto &key = item.key();
    NodeStack stack;
    walkTree(key, stack);
    auto treeNode = stack.back().first;

    // Ò»¶¨ÊÇleaf node;
    assert(treeNode->isLeaf());
    auto leafNode = std::static_pointer_cast<LeafNode>(treeNode);
    if (leafNode->item() == nullptr)
    {
        leafNode->setItem(item);
        return true;
    }
    else
    {
        auto leafNode = std::static_pointer_cast<LeafNode>(treeNode);
        assert(leafNode->item()->key() == item.key());
        return false;
    }
}

const Item::pointer & GZHMap::peekItem(const uint32 &key)
{
    NodeStack stack;
    walkTree(key, stack);

    auto treeNode = stack.back().first;
    auto leafNode = std::static_pointer_cast<LeafNode>(treeNode);
    return leafNode->item();
}
