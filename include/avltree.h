#ifndef __AVLTREE__
#define __AVLTREE__

#include <cassert>
#include <vector>
#include "table_commons.h"
#include "binary_tree_iterator.h"

template<typename K, typename V>
class AVLTree
{
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = Row<K, V>;
private:
    struct AVLTreeNode {
        value_type data;

        AVLTreeNode* left = nullptr;
        AVLTreeNode* right = nullptr;
        AVLTreeNode* parent = nullptr;
        size_t depth = 1;

        [[nodiscard]]
        int balance() const {
            return (left ? left->depth : 0) - (right ? right->depth : 0);
        };
        void update() {
            depth = std::max(left ? left->depth : 0, right ? right->depth : 0) + 1;
        }

        static void update_node(AVLTreeNode* node) {
            if (node != nullptr) node->update();
        }

        AVLTreeNode* rotate_left();
        AVLTreeNode* rotate_right();

        static AVLTreeNode* min(AVLTreeNode* subtree);
        static AVLTreeNode* max(AVLTreeNode* subtree);
    };

    using node_t = AVLTreeNode;

    node_t* root;
    size_t records;

    void destroy_nodes();
public:
    template<typename U>
    class Iterator;

    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;
private:
    void move_node(node_t* u, node_t* v);

    node_t* search_ancestor(const K& key);
    const node_t* search_ancestor(const K& key) const;

    node_t* lookup(const K& key);
    const node_t* lookup(const K& key) const;
    value_type& insert(const K& key, const V& value);
public:
    AVLTree();
    AVLTree(std::initializer_list<std::pair<K, V>> init);

    AVLTree(const AVLTree& src);
    AVLTree(AVLTree&& src) noexcept;

    ~AVLTree();

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

    AVLTree& operator=(const AVLTree& other);
    AVLTree& operator=(AVLTree&& other) noexcept;

    template<typename U>
    class Iterator : public BinaryTreeIterator<node_t, U>
    {
        friend class AVLTree;
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

    friend void swap(AVLTree& lhs, AVLTree& rhs) noexcept
    {
        std::swap(lhs.root, rhs.root);
        std::swap(lhs.records, rhs.records);
    }
};

template<typename K, typename V>
typename AVLTree<K, V>::AVLTreeNode* AVLTree<K, V>::AVLTreeNode::rotate_left()
{
    auto* _right = this->right;
    auto* _rleft = _right->left;

    _right->left = this;
    this->right = _rleft;

    _right->parent = this->parent;
    this->parent = _right;
    if(_rleft != nullptr) {
        _rleft->parent = this;
    }

    _right->update();
    this->update();

    return _right;
}
//
template<typename K, typename V>
typename AVLTree<K, V>::AVLTreeNode* AVLTree<K, V>::AVLTreeNode::rotate_right()
{
    auto* _left = this->left;
    auto* _lright = _left->right;

    _left->right = this;
    this->left = _lright;

    _left->parent = this->parent;
    this->parent = _left;
    if(_lright != nullptr) {
        _lright->parent = this;
    }

    _left->update();
    this->update();

    return _left;
}

template<typename K, typename V>
typename AVLTree<K, V>::AVLTreeNode* AVLTree<K, V>::AVLTreeNode::min(AVLTreeNode* subtree)
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
typename AVLTree<K, V>::AVLTreeNode* AVLTree<K, V>::AVLTreeNode::max(AVLTreeNode* subtree)
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
void AVLTree<K, V>::move_node(node_t* u, node_t* v)
{
    if (u->parent == nullptr) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != nullptr) {
        v->parent = u->parent;
    }
}

template<typename K, typename V>
AVLTree<K, V>::AVLTree()
    : records(0)
    , root(nullptr)
{}

template<typename K, typename V>
AVLTree<K, V>::AVLTree(std::initializer_list<std::pair<K, V>> init)
    : records(0)
    , root(nullptr)
{
    for (const auto& entry : init) {
        (*this)[entry.first] = entry.second;
    }
}

template<typename K, typename V>
AVLTree<K, V>::AVLTree(const AVLTree& src)
    : records(src.records)
{
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
AVLTree<K, V>::AVLTree(AVLTree&& src) noexcept
    : root(nullptr)
{
    swap(*this, src);
}
template<typename K, typename V>
void AVLTree<K, V>::destroy_nodes()
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
AVLTree<K, V>::~AVLTree()
{
    destroy_nodes();
}

template<typename K, typename V>
typename AVLTree<K, V>::node_t* AVLTree<K, V>::search_ancestor(const K& key)
{
    node_t *p = root, *prev = nullptr;

    while (p != nullptr) {
        prev = p;
        if (key < p->data.key) {
            p = p->left;
        } else if (key > p->data.key) {
            p = p->right;
        } else { // data == p->data
            return prev;
        }
    }

    return prev;
}

template<typename K, typename V>
const typename AVLTree<K, V>::node_t* AVLTree<K, V>::search_ancestor(const K& key) const
{
    return const_cast<AVLTree *>(this)->search_ancestor(key);
}

template<typename K, typename V>
typename AVLTree<K, V>::node_t* AVLTree<K, V>::lookup(const K& key)
{
    auto* parent = search_ancestor(key);

    if (parent == nullptr) {
        return nullptr;
    }

    if (parent->data.key == key) {
        // root
        return parent;
    }

    return (parent->left != nullptr && parent->left->data.key == key) ? parent->left : parent->right;
}

template<typename K, typename V>
const typename AVLTree<K, V>::node_t* AVLTree<K, V>::lookup(const K& key) const
{
    return const_cast<AVLTree*>(this)->lookup(key);
}

template<typename K, typename V>
typename AVLTree<K, V>::value_type& AVLTree<K, V>::insert(const K& key, const V& value)
{
    ++records;

    auto *child = new node_t { { key, value } };

    auto *prev = search_ancestor(key);

    if (prev == nullptr) {
        this->root = child;
    } else {
        if (child->data > prev->data) {
            prev->right = child;
        } else {
            prev->left = child;
        }
    }

    child->parent = prev;

    for (auto* p = prev; p != nullptr; p = p->parent) {
        const int bal = p->balance();

        auto* par = p->parent;
        if (bal > 1) {
            if (child->data > p->left->data) {
                p->left = p->left->rotate_left();
            }

            if (par != nullptr && par->right == p) {
                par->right = p->rotate_right();
            } else if (par != nullptr && par->left == p) {
                par->left = p->rotate_right();
            } else {
                p->rotate_right();
            }
        } else if (bal < -1) {
            if (child->data < p->right->data) {
                p->right = p->right->rotate_right();
            }

            if (par != nullptr && par->right == p) {
                par->right = p->rotate_left();
            } else if (par != nullptr && par->left == p) {
                par->left = p->rotate_left();
            } else {
                p->rotate_left();
            }
        } else {
            p->update();
        }

        if (p->parent == nullptr) {
            this->root = p;
        }
    }

    return child->data;
}

template<typename K, typename V>
bool AVLTree<K, V>::has(const K& key) const
{
    return lookup(key) != nullptr;
}

template<typename K, typename V>
void AVLTree<K, V>::erase(AVLTree::const_iterator pos)
{
    --records;

    auto* node = pos.cur;

    if (node->left == nullptr) {
        move_node(node, node->right);
    } else if (node->right == nullptr) {
        move_node(node, node->left);
    } else {
        auto* y = node_t::min(node->right);
        if (y->parent != node) {
            move_node(y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        move_node(node, y);
        y->left = node->left;
        y->left->parent = y;
    }

    delete node;
}

template<typename K, typename V>
void AVLTree<K, V>::erase(const K& key)
{
    auto* node = lookup(key);
    assert(node != nullptr && "Key not exists");
    erase(node);
}

template<typename K, typename V>
size_t AVLTree<K, V>::size() const
{
    return records;
}

template<typename K, typename V>
bool AVLTree<K, V>::empty() const
{
    return records == 0;
}

template<typename K, typename V>
void AVLTree<K, V>::clear()
{
    destroy_nodes();
    root = nullptr;
    records = 0;
}

template<typename K, typename V>
V& AVLTree<K, V>::at(const K& key)
{
    return const_cast<V&>(std::as_const(*this).at(key));
}

template<typename K, typename V>
const V& AVLTree<K, V>::at(const K& key) const
{
    const auto* node = lookup(key);
    assert(node != nullptr && "Key not exists");
    return node->data.value;
}

template<typename K, typename V>
V& AVLTree<K, V>::operator[](const K& key)
{
    auto* node = lookup(key);
    if (node != nullptr) {
        return node->data.value;
    }
    return insert(key, {}).value;
}

template<typename K, typename V>
AVLTree<K, V>& AVLTree<K, V>::operator=(const AVLTree& other)
{
    if (this == &other)
        return *this;

    AVLTree<K, V> tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename K, typename V>
AVLTree<K, V>& AVLTree<K, V>::operator=(AVLTree&& other) noexcept
{
    swap(*this, other);
    return *this;
}

template<typename K, typename V>
typename AVLTree<K, V>::iterator AVLTree<K, V>::begin()
{
    return { node_t::min(root) };
}
template<typename K, typename V>
typename AVLTree<K, V>::iterator AVLTree<K, V>::end()
{
    return { nullptr };
}
//
template<typename K, typename V>
typename AVLTree<K, V>::const_iterator AVLTree<K, V>::cbegin() const
{
    return { node_t::min(root) };
}
template<typename K, typename V>
typename AVLTree<K, V>::const_iterator AVLTree<K, V>::cend() const
{
    return { nullptr };
}

#endif // __AVLTREE__
