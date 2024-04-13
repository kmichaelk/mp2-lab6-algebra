#ifndef __BINARYTREE_BinaryTreeIterator_H__
#define __BINARYTREE_BinaryTreeIterator_H__

template<typename Node, typename U>
class BinaryTreeIterator
{
protected:
    Node* cur;

    BinaryTreeIterator(Node* node);
public:
    using value_type = U;

    BinaryTreeIterator& operator++();

    bool operator==(const BinaryTreeIterator& other) const;
    bool operator!=(const BinaryTreeIterator& other) const;

    U& operator*() const;
    U* operator->() const;
};

template<typename Node, typename U>
BinaryTreeIterator<Node, U>::BinaryTreeIterator(Node* node)
        : cur(node)
{}

template<typename Node, typename U>
BinaryTreeIterator<Node, U>& BinaryTreeIterator<Node, U>::operator++()
{
    if (cur->right != nullptr) {
        cur = cur->right;
        while (cur->left != nullptr) {
            cur = cur->left;
        }
    } else {
        auto* par = cur->parent;
        while (par != nullptr && cur == par->right) {
            cur = par;
            par = par->parent;
        }
        if (cur->right != par) {
            cur = par;
        }
    }

    return *this;
}

template<typename Node, typename U>
bool BinaryTreeIterator<Node, U>::operator==(const BinaryTreeIterator<Node, U>& other) const
{
    return cur == other.cur;
}

template<typename Node, typename U>
bool BinaryTreeIterator<Node, U>::operator!=(const BinaryTreeIterator<Node, U>& other) const
{
    return !(*this == other);
}

template<typename Node, typename U>
U& BinaryTreeIterator<Node, U>::operator*() const
{
    return cur->data;
}

template<typename Node, typename U>
U* BinaryTreeIterator<Node, U>::operator->() const
{
    return &cur->data;
}

#endif // __BINARYTREE_BinaryTreeIterator_H__
