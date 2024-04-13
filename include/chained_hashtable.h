#ifndef __CHAINED_HASHTABLE__
#define __CHAINED_HASHTABLE__

#include <cassert>
#include <vector>
#include "table_commons.h"
#include "chain.h"

template<typename K, typename V>
class ChainedHashTable
{
public:
    using key_type    = K;
    using mapped_type = V;
    using value_type  = Row<K, V>;
private:
    size_t capacity_preset = 0;
    size_t capacity;
    size_t records;

    std::vector<Chain<value_type>> mem;

    void grow_if_required(); // 0.75
public:
    template<typename U>
    class Iterator;

    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;
protected:
    iterator lookup(const K& key);
    const_iterator lookup(const K& key) const;
    value_type& insert(const K& key, const V& value);

    [[nodiscard]] inline size_t translate(const K& key, size_t c) const;
    [[nodiscard]] inline size_t translate(const K& key) const;
public:
    ChainedHashTable();
    ChainedHashTable(std::initializer_list<std::pair<K, V>> init);

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

    template<typename U>
    class Iterator
    {
        friend class ChainedHashTable;
    private:
        using mem_iterator   = typename decltype(mem)::iterator;
        using chain_iterator = typename decltype(mem)::value_type::iterator;

        mem_iterator it_m, it_m_end;
        chain_iterator it_c;

        Iterator(mem_iterator it_m, mem_iterator it_m_end,
                 chain_iterator it_c);
    public:
        using value_type = U;

        // iterator -> [const_]iterator
        Iterator(const iterator& it)
                : it_m(it.it_m)
                , it_m_end(it.it_m_end)
                , it_c(it.it_c)
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
};

template<typename K, typename V>
ChainedHashTable<K, V>::ChainedHashTable()
    : capacity_preset(0)
    , capacity(HashTableCommons::CAPACITIES[capacity_preset])
    , records(0)
    , mem(capacity)
{}

template<typename K, typename V>
ChainedHashTable<K, V>::ChainedHashTable(std::initializer_list<std::pair<K, V>> init)
    : capacity_preset(std::distance(std::begin(HashTableCommons::CAPACITIES), std::lower_bound(std::begin(HashTableCommons::CAPACITIES), std::end(HashTableCommons::CAPACITIES), init.size())))
    , capacity(HashTableCommons::CAPACITIES[capacity_preset])
    , records(0)
    , mem(capacity)
{
    for (const auto& entry : init) {
        (*this)[entry.first] = entry.second;
    }
}

template<typename K, typename V>
void ChainedHashTable<K, V>::grow_if_required()
{
    if ((4 * records) < (3 * capacity))
        return;

    const size_t new_capacity = (capacity_preset < HashTableCommons::CAPACITIES_COUNT - 1)
            ? HashTableCommons::CAPACITIES[++capacity_preset]
            : capacity * 2;

    decltype(mem) new_mem(new_capacity);
    for (const auto& bucket : mem) {
        for (const auto& link : bucket) {
            new_mem[translate(link.key, new_capacity)].emplace_back(link);
        }
    }

    std::swap(mem, new_mem);
    capacity = new_capacity;
}

template<typename K, typename V>
typename ChainedHashTable<K, V>::iterator ChainedHashTable<K, V>::lookup(const K& key)
{
    const auto it_m = mem.begin() + translate(key);
    for (auto it_c = it_m->begin(); it_c != it_m->end(); ++it_c) {
        if (it_c->key == key) {
            return iterator(it_m, mem.end(), it_c);
        }
    }
    return end();
}

template<typename K, typename V>
typename ChainedHashTable<K, V>::const_iterator ChainedHashTable<K, V>::lookup(const K& key) const
{
    return const_iterator(const_cast<ChainedHashTable*>(this)->lookup(key));
}

template<typename K, typename V>
typename ChainedHashTable<K, V>::value_type& ChainedHashTable<K, V>::insert(const K& key, const V& value)
{
    ++records;
    grow_if_required();
    return mem[translate(key)].emplace_back(key, value);
}

template<typename K, typename V>
inline size_t ChainedHashTable<K, V>::translate(const K& key, size_t c) const
{
    return std::hash<K>{}(key) % c;
}

template<typename K, typename V>
inline size_t ChainedHashTable<K, V>::translate(const K& key) const
{
    return translate(key, mem.size());
}

template<typename K, typename V>
bool ChainedHashTable<K, V>::has(const K& key) const
{
    return lookup(key) != end();
}

template<typename K, typename V>
void ChainedHashTable<K, V>::erase(ChainedHashTable::const_iterator pos)
{
    --records;
    (*const_cast<Chain<value_type>*>(&(*pos.it_m))).erase(pos.it_c);
}

template<typename K, typename V>
void ChainedHashTable<K, V>::erase(const K& key)
{
    const auto it = std::as_const(*this).lookup(key);
    assert(it != std::as_const(*this).end() && "Key not exists");
    erase(it);
}

template<typename K, typename V>
size_t ChainedHashTable<K, V>::size() const
{
    return records;
}

template<typename K, typename V>
bool ChainedHashTable<K, V>::empty() const
{
    return records == 0;
}

template<typename K, typename V>
void ChainedHashTable<K, V>::clear()
{
    mem.clear();
    mem.resize(capacity);
    records = 0;
}

template<typename K, typename V>
V& ChainedHashTable<K, V>::at(const K& key)
{
    return const_cast<V&>(std::as_const(*this).at(key));
}

template<typename K, typename V>
const V& ChainedHashTable<K, V>::at(const K& key) const
{
    const auto it = lookup(key);
    assert(it != end() && "Key not exists");
    return it->value;
}

template<typename K, typename V>
V& ChainedHashTable<K, V>::operator[](const K& key)
{
    auto it = lookup(key);
    if (it != end()) {
        return it->value;
    }
    return insert(key, {}).value;
}

template<typename K, typename V>
typename ChainedHashTable<K, V>::iterator ChainedHashTable<K, V>::begin()
{
    return iterator(mem.begin(), mem.end(), mem[0].begin());
}
template<typename K, typename V>
typename ChainedHashTable<K, V>::iterator ChainedHashTable<K, V>::end()
{
    return iterator(mem.end(), mem.end(), mem[0].end());
}
//
template<typename K, typename V>
typename ChainedHashTable<K, V>::const_iterator ChainedHashTable<K, V>::cbegin() const
{
    return const_iterator(
            const_cast<ChainedHashTable*>(this)->mem.begin(),
            const_cast<ChainedHashTable*>(this)->mem.end(),
            const_cast<ChainedHashTable*>(this)->mem[0].begin());
}
template<typename K, typename V>
typename ChainedHashTable<K, V>::const_iterator ChainedHashTable<K, V>::cend() const
{
    return const_iterator(
            const_cast<ChainedHashTable*>(this)->mem.end(),
            const_cast<ChainedHashTable*>(this)->mem.end(),
            const_cast<ChainedHashTable*>(this)->mem[0].end());
}

template<typename K, typename V>
template<typename U>
ChainedHashTable<K, V>::Iterator<U>::Iterator(typename ChainedHashTable::Iterator<U>::mem_iterator it_m,
                                              typename ChainedHashTable::Iterator<U>::mem_iterator it_m_end,
                                              typename ChainedHashTable::Iterator<U>::chain_iterator it_c)
    : it_m(it_m)
    , it_m_end(it_m_end)
    , it_c(it_c)
{
    while (this->it_m != this->it_m_end && this->it_c == this->it_m->end()) {
        ++(*this);
    }
}

template<typename K, typename V>
template<typename U>
typename ChainedHashTable<K, V>::template Iterator<U>& ChainedHashTable<K, V>::Iterator<U>::operator++()
{
    if (it_m == it_m_end) {
        ++it_c;
        return *this;
    }

    if (it_c != it_m->end()) {
        ++it_c;
        if (it_c != it_m->end()) {
            return *this;
        }
    }

    for (; it_m != it_m_end && it_c == it_m->end(); ++it_m) {
        it_c = it_m->begin();
    }

    return *this;
}

template<typename K, typename V>
template<typename U>
bool ChainedHashTable<K, V>::Iterator<U>::operator==(const typename ChainedHashTable<K, V>::template Iterator<U> &other) const
{
    return it_m == other.it_m && it_c == other.it_c;
}

template<typename K, typename V>
template<typename U>
bool ChainedHashTable<K, V>::Iterator<U>::operator!=(const typename ChainedHashTable<K, V>::template Iterator<U> &other) const
{
    return !(*this == other);
}

template<typename K, typename V>
template<typename U>
U& ChainedHashTable<K, V>::Iterator<U>::operator*() const
{
    return *it_c;
}

template<typename K, typename V>
template<typename U>
U* ChainedHashTable<K, V>::Iterator<U>::operator->() const
{
    return &*it_c;
}

#endif // __CHAINED_HASHTABLE__
