#ifndef __RBTREE__
#define __RBTREE__

#include <cassert>
#include <vector>
#include "table_commons.h"
#include "binary_tree_iterator.h"

template<typename K, typename V>
class RBTree
{
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = Row<K, V>;
private:
    struct RBTreeNode {
        typedef bool node_color_t;
        static constexpr node_color_t COLOR_RED   = false;
        static constexpr node_color_t COLOR_BLACK = true;

        value_type data;

        node_color_t color;

        RBTreeNode* left = nullptr;
        RBTreeNode* right = nullptr;
        RBTreeNode* parent = nullptr;

        RBTreeNode* successor();

        static RBTreeNode* min(RBTreeNode* subtree);
        static RBTreeNode* max(RBTreeNode* subtree);
    };

    using node_t = RBTreeNode;

    node_t* root;
    size_t records;

    void destroy_nodes();
public:
    template<typename U>
    class Iterator;

    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;
private:
    void node_rotate_left(node_t* node);
    void node_rotate_right(node_t* node);

    void fix_insertion(node_t* node);
    void fix_deletion(node_t* node);

    void erase(node_t* node);

    node_t* lookup(const K& key);
    const node_t* lookup(const K& key) const;
    value_type& insert(const K& key, const V& value);
public:
    RBTree();
    RBTree(std::initializer_list<std::pair<K, V>> init);

    RBTree(const RBTree& src);
    RBTree(RBTree&& src) noexcept;

    ~RBTree();

    [[nodiscard]]
    bool has(const K& key) const;

    void erase(const_iterator pos);
    void erase(const K& key);

    [[nodiscard]]
    size_t size() const;
    [[nodiscard]]
    bool empty() const;

    void clear();

    [[nodiscard]] V& at(const K& key);
    [[nodiscard]] const V& at(const K& key) const;

    [[nodiscard]] V& operator[](const K& key);

    RBTree& operator=(const RBTree& other);
    RBTree& operator=(RBTree&& other) noexcept;

    template<typename U>
    class Iterator : public BinaryTreeIterator<node_t, U>
    {
        friend class RBTree;
        Iterator(node_t* node) : BinaryTreeIterator<node_t, U>(node) {}
    };

    iterator begin();
    iterator end();
    //
    const_iterator begin() const { return cbegin(); };
    const_iterator end() const { return cend(); };
    //
    const_iterator cbegin() const;
    const_iterator cend() const;

    friend void swap(RBTree& lhs, RBTree& rhs) noexcept
    {
        std::swap(lhs.root, rhs.root);
        std::swap(lhs.records, rhs.records);
    }
};

template<typename K, typename V>
void RBTree<K, V>::node_rotate_left(node_t* node)
{
    auto* _right = node->right;
    node->right = _right->left;

    if (_right->left) {
        _right->left->parent = node;
    }
    _right->parent = node->parent;

    if (!node->parent) {
        root = _right;
    } else if (node == node->parent->left) {
        node->parent->left = _right;
    } else {
        node->parent->right = _right;
    }

    _right->left = node;
    node->parent = _right;
}
//
template<typename K, typename V>
void RBTree<K, V>::node_rotate_right(node_t* node)
{
    auto* _left = node->left;
    node->left = _left->right;

    if (_left->right) {
        _left->right->parent = node;
    }
    _left->parent = node->parent;

    if (!node->parent) {
        root = _left;
    } else if (node == node->parent->right) {
        node->parent->right = _left;
    } else {
        node->parent->left = _left;
    }

    _left->right = node;
    node->parent = _left;
}

template<typename K, typename V>
void RBTree<K, V>::fix_insertion(node_t* node)
{
    node_t *grandparent, *uncle;
    while (node != root && node->parent->color == node_t::COLOR_RED) {
        grandparent = node->parent->parent;
        if (node->parent == grandparent->left) {
            uncle = grandparent->right;
            if (uncle != nullptr && uncle->color == node_t::COLOR_RED) {
                node->parent->color = node_t::COLOR_BLACK;
                uncle->color = node_t::COLOR_BLACK;
                grandparent->color = node_t::COLOR_RED;
                node = grandparent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    node_rotate_left(node);
                }
                node->parent->color = node_t::COLOR_BLACK;
                grandparent->color = node_t::COLOR_RED;
                node_rotate_right(grandparent);
            }
        } else {
            uncle = grandparent->left;
            if (uncle != nullptr && uncle->color == node_t::COLOR_RED) {
                node->parent->color = node_t::COLOR_BLACK;
                uncle->color = node_t::COLOR_BLACK;
                grandparent->color = node_t::COLOR_RED;
                node = grandparent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    node_rotate_right(node);
                }
                node->parent->color = node_t::COLOR_BLACK;
                grandparent->color = node_t::COLOR_RED;
                node_rotate_left(grandparent);
            }
        }
    }
    root->color = node_t::COLOR_BLACK;
}

template<typename K, typename V>
void RBTree<K, V>::fix_deletion(node_t* node)
{
    node_t *sibling;
    while (node != root && node->color == node_t::COLOR_BLACK) {
        if (node == node->parent->left) {
            sibling = node->parent->right;
            if (sibling == nullptr) {
                continue;
            }
            if (sibling->color == node_t::COLOR_RED) {
                sibling->color = node_t::COLOR_BLACK;
                node->parent->color = node_t::COLOR_RED;
                node_rotate_left(node->parent);
                sibling = node->parent->right;
            }
            if (sibling != nullptr && sibling->right != nullptr && sibling->left != nullptr) {
                if (sibling->right->color == node_t::COLOR_BLACK && sibling->left->color == node_t::COLOR_BLACK) {
                    sibling->color = node_t::COLOR_RED;
                    node = node->parent;
                } else {
                    if (sibling->right != nullptr && sibling->right->color == node_t::COLOR_BLACK) {
                        sibling->left->color = node_t::COLOR_BLACK;
                        sibling->color = node_t::COLOR_RED;
                        node_rotate_right(sibling);
                        sibling = node->parent->right;
                    }
                    if (sibling != nullptr && sibling->right != nullptr && sibling->right->color == node_t::COLOR_RED) {
                        sibling->color = node->parent->color;
                        node->parent->color = node_t::COLOR_BLACK;
                        sibling->right->color = node_t::COLOR_BLACK;
                        node_rotate_left(node->parent);
                        node = root;
                    }
                }
            }
        } else {
            sibling = node->parent->left;
            if (sibling == nullptr) {
                continue;
            }
            if (sibling->color == node_t::COLOR_RED) {
                sibling->color = node_t::COLOR_BLACK;
                node->parent->color = node_t::COLOR_RED;
                node_rotate_right(node->parent);
                sibling = node->parent->left;
            }
            if (sibling != nullptr && sibling->right != nullptr && sibling->left != nullptr) {
                if (sibling->right->color == node_t::COLOR_BLACK && sibling->left->color == node_t::COLOR_BLACK) {
                    sibling->color = node_t::COLOR_RED;
                    node = node->parent;
                } else {
                    if (sibling->left != nullptr && sibling->left->color == node_t::COLOR_BLACK) {
                        sibling->right->color = node_t::COLOR_BLACK;
                        sibling->color = node_t::COLOR_RED;
                        node_rotate_left(sibling);
                        sibling = node->parent->left;
                    }
                    if (sibling != nullptr && sibling->left != nullptr && sibling->left->color == node_t::COLOR_RED) {
                        sibling->color = node->parent->color;
                        node->parent->color = node_t::COLOR_BLACK;
                        sibling->left->color = node_t::COLOR_BLACK;
                        node_rotate_right(node->parent);
                        node = root;
                    }
                }
            }
        }
    }
    node->color = node_t::COLOR_BLACK;
}

template<typename K, typename V>
typename RBTree<K, V>::RBTreeNode* RBTree<K, V>::RBTreeNode::min(RBTreeNode* subtree)
{
    if (subtree == nullptr) {
        return nullptr;
    }

    auto* p = subtree;
    while (p->left != nullptr) {
        p = p->left;
    }
    return p;
}

template<typename K, typename V>
typename RBTree<K, V>::RBTreeNode* RBTree<K, V>::RBTreeNode::max(RBTreeNode* subtree)
{
    if (subtree == nullptr) {
        return nullptr;
    }

    auto* p = subtree;
    while (p->right != nullptr) {
        p = p->right;
    }
    return p;
}

template<typename K, typename V>
typename RBTree<K, V>::RBTreeNode* RBTree<K, V>::RBTreeNode::successor()
{
    node_t* node;

    if (this->right != nullptr) {
        node = this->right;
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    auto* par = this->parent;
    while (par != nullptr && node == parent->right) {
        node = par;
        par = par->parent;
    }
    return par;
}

template<typename K, typename V>
RBTree<K, V>::RBTree()
    : records(0)
    , root(nullptr)
{}

template<typename K, typename V>
RBTree<K, V>::RBTree(std::initializer_list<std::pair<K, V>> init)
    : records(0)
    , root(nullptr)
{
    for (const auto& entry : init) {
        (*this)[entry.first] = entry.second;
    }
}

template<typename K, typename V>
RBTree<K, V>::RBTree(const RBTree& src)
    : root(nullptr)
    , records(src.records)
{
    if (src.root == nullptr) return;

    const node_t* p_src = src.root;

    root = new node_t { src.root->data };
    node_t* clone = root;

    while (p_src != nullptr)
    {
        if (p_src->left != nullptr && clone->left == nullptr) {
            clone->left = new node_t { p_src->left->data };
            clone->left->parent = clone;
            p_src = p_src->left;
            clone = clone->left;
        } else if (p_src->right != nullptr && clone->right == nullptr) {
            clone->right = new node_t { p_src->right->data };
            clone->right->parent = clone;
            p_src = p_src->right;
            clone = clone->right;
        } else {
            p_src = p_src->parent;
            clone = clone->parent;
        }
    }

}

template<typename K, typename V>
RBTree<K, V>::RBTree(RBTree&& src) noexcept
    : root(nullptr)
{
    swap(*this, src);
}
template<typename K, typename V>
void RBTree<K, V>::destroy_nodes()
{
    node_t* save;

    for (node_t* it = root; it != NULL; it = save) {
        if (it->left == NULL) {
            save = it->right;
            delete it;
        } else {
            save = it->left;
            it->left = save->right;
            save->right = it;
        }
    }
}

template<typename K, typename V>
RBTree<K, V>::~RBTree()
{
    destroy_nodes();
}

template<typename K, typename V>
typename RBTree<K, V>::node_t* RBTree<K, V>::lookup(const K& key)
{
    node_t *p = root;

    while (p != nullptr) {
        if (key < p->data.key) {
            p = p->left;
        } else if (key > p->data.key) {
            p = p->right;
        } else { // data == p->data
            return p;
        }
    }

    return nullptr;
}

template<typename K, typename V>
const typename RBTree<K, V>::node_t* RBTree<K, V>::lookup(const K& key) const
{
    return const_cast<RBTree*>(this)->lookup(key);
}

template<typename K, typename V>
typename RBTree<K, V>::value_type& RBTree<K, V>::insert(const K& key, const V& value)
{
    ++records;

    auto* child = new node_t { { key, value } };
    child->color = node_t::COLOR_RED;

    node_t *cur = root, *par = nullptr;
    while (cur != nullptr) {
        par = cur;
        if (key < cur->data.key) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }

    child->parent = par;

    if (par == nullptr) {
        root = child;
    } else if (key < par->data.key) {
        par->left = child;
    } else {
        par->right = child;
    }

    fix_insertion(child);

    return child->data;
}

template<typename K, typename V>
bool RBTree<K, V>::has(const K& key) const
{
    return lookup(key) != nullptr;
}

template<typename K, typename V>
void RBTree<K, V>::erase(RBTree::node_t *node)
{
    if (node == nullptr)
        return;

    if (node->left != nullptr && node->right != nullptr) {
        auto* successor = node->right->successor();
        node->data = std::move(successor->data);
        erase(node->right);
        node->right = nullptr;
    } else {
        auto* child = node->left ? node->left : node->right;

        if (child == nullptr) {
            child = node;
        } else {
            *node = *child;
        }

        delete child;
        --records;

        if (records == 0) {
            root = nullptr;
        }
    }

    fix_deletion(node);
}

template<typename K, typename V>
void RBTree<K, V>::erase(RBTree::const_iterator pos)
{
    erase(pos.cur);
}

template<typename K, typename V>
void RBTree<K, V>::erase(const K& key)
{
    auto* node = lookup(key);
    assert(node != nullptr && "Key not exists");
    erase(node);
}

template<typename K, typename V>
size_t RBTree<K, V>::size() const
{
    return records;
}

template<typename K, typename V>
bool RBTree<K, V>::empty() const
{
    return records == 0;
}

template<typename K, typename V>
void RBTree<K, V>::clear()
{
    destroy_nodes();
    root = nullptr;
    records = 0;
}

template<typename K, typename V>
V& RBTree<K, V>::at(const K& key)
{
    return const_cast<V&>(std::as_const(*this).at(key));
}

template<typename K, typename V>
const V& RBTree<K, V>::at(const K& key) const
{
    const auto* node = lookup(key);
    assert(node != nullptr && "Key not exists");
    return node->data.value;
}

template<typename K, typename V>
V& RBTree<K, V>::operator[](const K& key)
{
    auto* node = lookup(key);
    if (node != nullptr) {
        return node->data.value;
    }
    return insert(key, {}).value;
}

template<typename K, typename V>
RBTree<K, V>& RBTree<K, V>::operator=(const RBTree& other)
{
    if (this == &other)
        return *this;

    RBTree<K, V> tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename K, typename V>
RBTree<K, V>& RBTree<K, V>::operator=(RBTree&& other) noexcept
{
    swap(*this, other);
    return *this;
}

template<typename K, typename V>
typename RBTree<K, V>::iterator RBTree<K, V>::begin()
{
    return { node_t::min(root) };
}
template<typename K, typename V>
typename RBTree<K, V>::iterator RBTree<K, V>::end()
{
    return { nullptr };
}
//
template<typename K, typename V>
typename RBTree<K, V>::const_iterator RBTree<K, V>::cbegin() const
{
    return { node_t::min(root) };
}
template<typename K, typename V>
typename RBTree<K, V>::const_iterator RBTree<K, V>::cend() const
{
    return { nullptr };
}

#endif // __RBTREE__
