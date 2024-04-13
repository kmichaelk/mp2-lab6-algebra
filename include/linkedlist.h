#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdexcept>
#include <cassert>

template<typename T>
class LinkedList {
protected:
    size_t length;
    struct Node {
        T value;
        Node *next = nullptr;
    } *first, *last;

    mutable struct {
        size_t idx;
        Node *node;
    } cache;

    Node* get_node_unamortized(size_t idx) const;
    Node* get_node(size_t idx) const;

private:
    void destroy_nodes();

public:
    using value_type = T;

    LinkedList();
    LinkedList(std::initializer_list<T> init);

    LinkedList(const LinkedList& src);
    LinkedList(LinkedList&& src) noexcept;

    ~LinkedList();

    template<typename U>
    class Iterator;
    //
    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;

    [[nodiscard]]
    size_t size() const noexcept;
    [[nodiscard]]
    bool empty() const noexcept;

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args);
    template<class... Args>
    iterator emplace(size_t pos, Args&&... args);
    template<class... Args>
    T& emplace_back(Args&&... args);

    void push_back(const T& element);
    void push_back(T&& element);
    //
    void push_front(const T& element);
    void push_front(T&& element);

    void insert(const_iterator pos, const T& element);
    void insert(const_iterator pos, T&& element);
    void insert(size_t pos, const T& element);
    void insert(size_t pos, T&& element);
    //
    void erase(const_iterator pos);
    void erase(size_t pos);

    void clear();

    T& front();
    T& back();

    T& at(size_t idx);
    const T& at(size_t idx) const;

    T& operator[](size_t idx);
    const T& operator[](size_t idx) const;

    bool operator==(const LinkedList& other) const noexcept;
    bool operator!=(const LinkedList& other) const noexcept;

    LinkedList& operator=(const LinkedList& other);
    LinkedList& operator=(LinkedList&& other) noexcept;

    template<typename U>
    class Iterator
    {
        friend class LinkedList;
    private:
        Node* cur;
        Node* prev;
        size_t idx;

        Iterator(Node* node, size_t idx, Node* prev);
    public:
        using value_type = U;

        // iterator -> [const_]iterator
        Iterator(const iterator& it)
                : cur(it.cur)
                , prev(it.prev)
                , idx(it.idx)
        {}

        Iterator& operator++();

        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

        U& operator*() const;
        U* operator->() const;
    };

    iterator begin();
    iterator end();
    //
    const_iterator begin() const { return cbegin(); };
    const_iterator end() const { return cend(); };
    //
    const_iterator cbegin() const;
    const_iterator cend() const;

    friend void swap(LinkedList& lhs, LinkedList& rhs) noexcept
    {
        std::swap(lhs.length, rhs.length);
        std::swap(lhs.first, rhs.first);
        std::swap(lhs.last, rhs.last);
        std::swap(lhs.cache, rhs.cache);
    }
};

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::get_node_unamortized(size_t idx) const
{
    Node *node = first;
    for (size_t i = 0; i < idx; i++)
        node = node->next;
    return node;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::get_node(size_t idx) const
{
    size_t i;
    Node *node;

    if (cache.idx <= idx)
    {
        i = cache.idx;
        node = cache.node;
    } else {
        i = 0;
        node = first;
    }

    for ( ; i < idx; i++) {
        node = node->next;
    }

    cache.idx = idx;
    cache.node = node;

    return node;
}

template<typename T>
void LinkedList<T>::destroy_nodes()
{
    Node *current = first, *garbage;
    while (current)
    {
        garbage = current;
        current = garbage->next;
        delete garbage;
    }
}

template<typename T>
LinkedList<T>::LinkedList()
        : length(0)
        , first(nullptr)
        , last(nullptr)
        , cache({ 0, nullptr })
{}

template<typename T>
LinkedList<T>::LinkedList(std::initializer_list<T> init)
        : length(init.size())
        , first(nullptr)
        , last(nullptr)
        , cache({ 0, nullptr })
{
    if (init.size() == 0)
        return;

    auto it = init.begin();

    Node *cur = first = new Node{ *it++, nullptr };
    for (; it != init.end(); ++it) {
        cur->next = new Node { *it, nullptr };
        cur = cur->next;
    }
    last = cur;

    cache.node = first;
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& src)
        : length(src.length)
        , first(nullptr)
        , last(nullptr)
        , cache({ 0, nullptr })
{
    if (!src.first) {
        return;
    }

    Node *cur = first = new Node{ *src.first };
    for (; cur->next; cur = cur->next) {
        cur->next = new Node{ *cur->next };
    }
    last = cur;

    cache.node = first;
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList&& src) noexcept
        : first(nullptr)
        , last(nullptr)
{
    swap(*this, src);
}

template<typename T>
LinkedList<T>::~LinkedList()
{
    destroy_nodes();
}

template<typename T>
size_t LinkedList<T>::size() const noexcept
{
    return length;
}

template<typename T>
bool LinkedList<T>::empty() const noexcept
{
    return length == 0;
}

template<typename T>
template<class... Args>
typename LinkedList<T>::iterator LinkedList<T>::emplace(const_iterator pos, Args&&... args)
{
    auto* node = new Node { T(std::forward<Args>(args)...), pos.cur };

    if (pos.prev == nullptr) {
        // pos.cur == first
        first = node;
    } else {
        pos.prev->next = node;
    }

    if (pos.prev == last) {
        last = node;
    }

    length++;

    if (pos.idx <= cache.idx) {
        ++cache.idx;
    }

    if (!cache.node) {
        cache.idx = 0;
        cache.node = first;
    }

    return iterator(node, pos.idx, pos.prev);
}


template<typename T>
template<class... Args>
typename LinkedList<T>::iterator LinkedList<T>::emplace(size_t pos, Args&&... args)
{
    if (pos == 0) {
        return emplace({ first, pos, nullptr }, args...);
    }

    Node* prev = get_node(pos - 1);
    return emplace({ prev->next, pos, prev }, args...);
}

template<typename T>
template<class... Args>
T& LinkedList<T>::emplace_back(Args&&... args)
{
    return *emplace({ nullptr, length, last }, args...);
}

template<typename T>
void LinkedList<T>::push_back(const T& element)
{
    emplace_back(element);
}

template<typename T>
void LinkedList<T>::push_back(T&& element)
{
    emplace_back(std::move(element));
}

template<typename T>
void LinkedList<T>::push_front(const T& element)
{
    emplace({ first, 0, nullptr }, element);
}

template<typename T>
void LinkedList<T>::push_front(T&& element)
{
    emplace({ first, 0, nullptr }, std::move(element));
}

template<typename T>
void LinkedList<T>::insert(LinkedList::const_iterator pos, const T& element)
{
    emplace(pos, element);
}

template<typename T>
void LinkedList<T>::insert(LinkedList::const_iterator pos, T&& element)
{
    emplace(pos, element);
}

template<typename T>
void LinkedList<T>::insert(size_t pos, const T& element)
{
    assert(pos < length && "Index is out of range");

    emplace(pos, element);
}

template<typename T>
void LinkedList<T>::insert(size_t pos, T&& element)
{
    assert(pos < length && "Index is out of range");

    emplace(pos, element);
}

template<typename T>
void LinkedList<T>::erase(LinkedList::const_iterator pos)
{
    auto* node = pos.cur;

    if (pos.prev == nullptr) {
        // pos.cur == first
        first = pos.cur->next;
    } else {
        pos.prev->next = node->next;
    }

    length--;

    if (pos.idx == cache.idx)
    {
        cache.node = cache.node->next;
        if (!cache.node) {
            cache.node = first;
            cache.idx = 0;
        }
    } else if (pos.idx < cache.idx) {
        cache.idx--;
    }

    delete node;
}

template<typename T>
void LinkedList<T>::erase(size_t pos)
{
    if (pos == 0) {
        erase({ first, pos, nullptr });
        return;
    }

    assert(pos < length && "Index is out of range");

    Node* prev = get_node(pos - 1);
    erase({ prev->next, pos, prev });
}

template<typename T>
T& LinkedList<T>::front()
{
    assert(length && "List is empty");
    return first->value;
}

template<typename T>
T& LinkedList<T>::back()
{
    assert(length && "List is empty");
    return last->value;
}

template<typename T>
T& LinkedList<T>::at(const size_t idx)
{
    return const_cast<T&>(std::as_const(*this).at(idx));
}

template<typename T>
const T& LinkedList<T>::at(const size_t idx) const
{
    return (*this)[idx];
}

template<typename T>
void LinkedList<T>::clear()
{
    destroy_nodes();
    first = last = nullptr;
    length = 0;
}

template<typename T>
T& LinkedList<T>::operator[](const size_t idx)
{
    return const_cast<T&>(std::as_const(*this)[idx]);
}

template<typename T>
const T& LinkedList<T>::operator[](const size_t idx) const
{
    assert(idx < length && "Index is out of range"); // stripped out from release
    return get_node(idx)->value;
}

template<typename T>
bool LinkedList<T>::operator==(const LinkedList<T>& other) const noexcept
{
    if (this == &other)
        return true;

    if (length != other.length)
        return false;

    Node *left = first, *right = other.first;
    while (left)
    {
        if (left->value != right->value)
            return false;

        left = left->next;
        right = right->next;
    }

    return true;
}

template<typename T>
bool LinkedList<T>::operator!=(const LinkedList<T>& other) const noexcept
{
    return !(*this == other);
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other)
{
    if (this == &other)
        return *this;

    LinkedList<T> tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(LinkedList &&other) noexcept
{
    swap(*this, other);
    return *this;
}

// region Iterator Support

template<typename T>
typename LinkedList<T>::iterator LinkedList<T>::begin()
{
    return { first, 0, nullptr };
}

template<typename T>
typename LinkedList<T>::iterator LinkedList<T>::end()
{
    return { nullptr, length, last };
}

template<typename T>
typename LinkedList<T>::const_iterator LinkedList<T>::cbegin() const
{
    return { first, 0, nullptr };
}

template<typename T>
typename LinkedList<T>::const_iterator LinkedList<T>::cend() const
{
    return { nullptr, length, last };
}
// endregion

// region Iterator

template<typename T>
template<typename U>
LinkedList<T>::Iterator<U>::Iterator(LinkedList::Node *node, size_t idx, LinkedList::Node *prev)
        : cur(node)
        , idx(idx)
        , prev(prev)
{}

template<typename T>
template<typename U>
typename LinkedList<T>::template Iterator<U>& LinkedList<T>::Iterator<U>::operator++()
{
    prev = cur;
    cur = cur->next;
    ++idx;
    return *this;
}

template<typename T>
template<typename U>
bool LinkedList<T>::Iterator<U>::operator==(const typename LinkedList<T>::template Iterator<U> &other) const
{
    return cur == other.cur;
}

template<typename T>
template<typename U>
bool LinkedList<T>::Iterator<U>::operator!=(const typename LinkedList<T>::template Iterator<U> &other) const
{
    return !(*this == other);
}

template<typename T>
template<typename U>
U& LinkedList<T>::Iterator<U>::operator*() const
{
    return cur->value;
}

template<typename T>
template<typename U>
U* LinkedList<T>::Iterator<U>::operator->() const
{
    return &cur->value;
}

// endregion

#endif // __LINKEDLIST_H__
