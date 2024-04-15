#ifndef __CHAIN_H__
#define __CHAIN_H__

#include <stdexcept>
#include <cassert>

// lightweight linked list implementation
// to use with chained hash table
//  -- it does not store length and tail pointer

template<typename T>
class Chain {
private:
    struct Link {
        T value;
        Link *next = nullptr;
    } *head;

    void destroy_links();
public:
    using value_type = T;

    Chain();

    Chain(Chain&& src) noexcept;

    Chain(const Chain& src);
    Chain& operator=(const Chain& other);

    ~Chain();

    template<typename U>
    class Iterator;
    //
    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;

    template<class... Args>
    T& emplace_back(Args&&... args);

    void push_back(const T& element);
    void push_back(T&& element);

    void erase(const_iterator pos);

    void clear();

    [[nodiscard]] bool empty() const noexcept;

    template<typename U>
    class Iterator
    {
        friend class Chain;
    private:
        Link* cur;
        Link* prev;

        Iterator(Link* link);
    public:
        using value_type = U;

        // iterator -> [const_]iterator
        Iterator(const iterator& it)
                : cur(it.cur)
                , prev(it.prev)
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

    friend void swap(Chain& lhs, Chain& rhs) noexcept
    {
        std::swap(lhs.head, rhs.head);
    }
};

template<typename T>
void Chain<T>::destroy_links()
{
    Link *current = head, *garbage;
    while (current)
    {
        garbage = current;
        current = garbage->next;
        delete garbage;
    }
}

template<typename T>
Chain<T>::Chain()
    : head(nullptr)
{}

template<typename T>
Chain<T>::Chain(Chain &&src) noexcept
    : head(nullptr)
{
    swap(*this, src);
}

template<typename T>
Chain<T>::Chain(const Chain& src)
    : head(nullptr)
{
    if (!src.head) {
        return;
    }

    Link *cur = head = new Link{ *src.head };
    for (; cur->next; cur = cur->next) {
        cur->next = new Link{ *cur->next };
    }
}

template<typename T>
Chain<T>& Chain<T>::operator=(const Chain &other)
{
    if (this == &other)
        return *this;

    Chain<T> tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename T>
Chain<T>::~Chain()
{
    destroy_links();
}

template<typename T>
template<class... Args>
T& Chain<T>::emplace_back(Args&&... args)
{
    head = new Link { T(std::forward<Args>(args)...), head };
    return head->value;
}

template<typename T>
void Chain<T>::push_back(const T& element)
{
    emplace_back(element);
}

template<typename T>
void Chain<T>::push_back(T&& element)
{
    emplace_back(std::move(element));
}

template<typename T>
void Chain<T>::erase(const_iterator pos)
{
    if (pos.prev == nullptr) {
        delete head;
        head = nullptr;
        return;
    }

    Link* dead = pos.cur;
    pos.prev->next = dead->next;
    delete dead;
}

template<typename T>
void Chain<T>::clear()
{
    destroy_links();
    head = nullptr;
}

template<typename T>
bool Chain<T>::empty() const noexcept
{
    return head == nullptr;
}

// region Iterator Support

template<typename T>
typename Chain<T>::iterator Chain<T>::begin()
{
    return head;
}

template<typename T>
typename Chain<T>::iterator Chain<T>::end()
{
    return nullptr;
}

template<typename T>
typename Chain<T>::const_iterator Chain<T>::cbegin() const
{
    return head;
}

template<typename T>
typename Chain<T>::const_iterator Chain<T>::cend() const
{
    return nullptr;
}

// endregion

// region Iterator

template<typename T>
template<typename U>
Chain<T>::Iterator<U>::Iterator(Chain::Link *node)
        : cur(node)
        , prev(nullptr)
{}

template<typename T>
template<typename U>
typename Chain<T>::template Iterator<U>& Chain<T>::Iterator<U>::operator++()
{
    prev = cur;
    cur = cur->next;
    return *this;
}

template<typename T>
template<typename U>
bool Chain<T>::Iterator<U>::operator==(const typename Chain<T>::template Iterator<U> &other) const
{
    return cur == other.cur;
}

template<typename T>
template<typename U>
bool Chain<T>::Iterator<U>::operator!=(const typename Chain<T>::template Iterator<U> &other) const
{
    return !(*this == other);
}

template<typename T>
template<typename U>
U& Chain<T>::Iterator<U>::operator*() const
{
    return cur->value;
}

template<typename T>
template<typename U>
U* Chain<T>::Iterator<U>::operator->() const
{
    return &cur->value;
}

// endregion

#endif // __CHAIN_H__
