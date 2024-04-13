#ifndef __DOUBLEHASHEDTABLE__
#define __DOUBLEHASHEDTABLE__

#include <cassert>
#include <vector>
#include "table_commons.h"

// todo: rewrite using placement new to
//       actually destruct elements on erase

template<typename K, typename V>
class DoubleHashedTable
{
private:
    typedef bool row_status_t;
    static const row_status_t ROW_FREE = false;
    static const row_status_t ROW_BUSY = true;
    
    using record_type = std::pair<row_status_t, Row<K, V>>;
public:
    using key_type    = K;
    using mapped_type = V;
    using value_type  = typename record_type::second_type;
private:
    size_t capacity_preset = 0;
    size_t capacity;
    size_t records;

    std::vector<record_type> mem;

    void grow_if_required(); // 0.75
public:
    template<typename U>
    class Iterator;

    using iterator       = Iterator<value_type>;
    using const_iterator = Iterator<std::add_const_t<value_type>>;
protected:
    record_type* lookup(const K& key);
    const record_type* lookup(const K& key) const;
    value_type& insert(const K& key, const V& value);

    [[nodiscard]] inline size_t translate(const K& key, size_t p, size_t c) const;
    [[nodiscard]] inline size_t translate(const K& key, size_t p = 0) const;
public:
    DoubleHashedTable();
    DoubleHashedTable(std::initializer_list<std::pair<K, V>> init);

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
        friend class DoubleHashedTable;
    private:
        using mem_iterator = typename decltype(mem)::iterator;

        mem_iterator it_m, it_m_end;

        Iterator(mem_iterator it_m, mem_iterator it_m_end);
    public:
        using value_type = U;

        // iterator -> [const_]iterator
        Iterator(const iterator& it)
                : it_m(it.it_m)
                , it_m_end(it.it_m_end)
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
DoubleHashedTable<K, V>::DoubleHashedTable()
        : capacity_preset(0)
        , capacity(HashTableCommons::CAPACITIES[capacity_preset])
        , records(0)
        , mem(capacity)
{}

template<typename K, typename V>
DoubleHashedTable<K, V>::DoubleHashedTable(std::initializer_list<std::pair<K, V>> init)
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
void DoubleHashedTable<K, V>::grow_if_required()
{
    if ((4 * records) <= (3 * capacity))
        return;

    const size_t new_capacity = (capacity_preset < HashTableCommons::CAPACITIES_COUNT - 1)
                                ? HashTableCommons::CAPACITIES[++capacity_preset]
                                : capacity * 2;

    decltype(mem) new_mem(new_capacity);
    for (const auto& row : mem) {
        if (row.first == ROW_FREE) {
            continue;
        }

        size_t p = 0;
        typename decltype(mem)::iterator it_m;

        do {
            it_m = new_mem.begin() + translate(row.second.key, p++, new_capacity);
        } while (it_m->first != ROW_FREE);

        it_m->first = ROW_BUSY;
        it_m->second = row.second;
    }

    std::swap(mem, new_mem);
    capacity = new_capacity;
}

template<typename K, typename V>
typename DoubleHashedTable<K, V>::record_type* DoubleHashedTable<K, V>::lookup(const K& key)
{
    size_t p = 0;
    typename decltype(mem)::iterator it_m;

    do {
        it_m = mem.begin() + translate(key, p++);
        if (it_m->first == ROW_FREE) {
            return nullptr;
        }
    } while (it_m->second.key != key);

    return &*it_m;
}

template<typename K, typename V>
const typename DoubleHashedTable<K, V>::record_type* DoubleHashedTable<K, V>::lookup(const K& key) const
{
    return const_cast<DoubleHashedTable*>(this)->lookup(key);
}

template<typename K, typename V>
typename DoubleHashedTable<K, V>::value_type& DoubleHashedTable<K, V>::insert(const K& key, const V& value)
{
    ++records;
    grow_if_required();

    size_t p = 0;
    typename decltype(mem)::iterator it_m;

    do {
        it_m = mem.begin() + translate(key, p++);
    } while (it_m->first != ROW_FREE);

    it_m->first = ROW_BUSY;
    //
    it_m->second.key = key;
    it_m->second.value = value;

    return it_m->second;
}

template<typename K, typename V>
inline size_t DoubleHashedTable<K, V>::translate(const K& key, size_t p, size_t c) const
{
    return (std::hash<K>{}(key) + p) % c;
}

template<typename K, typename V>
inline size_t DoubleHashedTable<K, V>::translate(const K& key, size_t p) const
{
    return translate(key, p, mem.size());
}

template<typename K, typename V>
bool DoubleHashedTable<K, V>::has(const K& key) const
{
    return lookup(key) != nullptr;
}

template<typename K, typename V>
void DoubleHashedTable<K, V>::erase(DoubleHashedTable::const_iterator pos)
{
    --records;
    pos.it_m->first = ROW_FREE;
}

template<typename K, typename V>
void DoubleHashedTable<K, V>::erase(const K& key)
{
    auto* bucket = lookup(key);
    assert(bucket != nullptr && "Key not exists");

    --records;
    bucket->first = ROW_FREE;
}

template<typename K, typename V>
size_t DoubleHashedTable<K, V>::size() const
{
    return records;
}

template<typename K, typename V>
bool DoubleHashedTable<K, V>::empty() const
{
    return records == 0;
}

template<typename K, typename V>
void DoubleHashedTable<K, V>::clear()
{
    mem.clear();
    mem.resize(capacity);
    records = 0;
}

template<typename K, typename V>
V& DoubleHashedTable<K, V>::at(const K& key)
{
    return const_cast<V&>(std::as_const(*this).at(key));
}

template<typename K, typename V>
const V& DoubleHashedTable<K, V>::at(const K& key) const
{
    const auto* bucket = lookup(key);
    assert(bucket != nullptr && "Key not exists");
    return bucket->second.value;
}

template<typename K, typename V>
V& DoubleHashedTable<K, V>::operator[](const K& key)
{
    auto* bucket = lookup(key);
    if (bucket != nullptr) {
        return bucket->second.value;
    }
    return insert(key, {}).value;
}

template<typename K, typename V>
typename DoubleHashedTable<K, V>::iterator DoubleHashedTable<K, V>::begin()
{
    return iterator(mem.begin(), mem.end());
}
template<typename K, typename V>
typename DoubleHashedTable<K, V>::iterator DoubleHashedTable<K, V>::end()
{
    return iterator(mem.end(), mem.end());
}
//
template<typename K, typename V>
typename DoubleHashedTable<K, V>::const_iterator DoubleHashedTable<K, V>::cbegin() const
{
    return const_iterator(
            const_cast<DoubleHashedTable*>(this)->mem.begin(),
            const_cast<DoubleHashedTable*>(this)->mem.end());
}
template<typename K, typename V>
typename DoubleHashedTable<K, V>::const_iterator DoubleHashedTable<K, V>::cend() const
{
    return const_iterator(
            const_cast<DoubleHashedTable*>(this)->mem.end(),
            const_cast<DoubleHashedTable*>(this)->mem.end());
}

template<typename K, typename V>
template<typename U>
DoubleHashedTable<K, V>::Iterator<U>::Iterator(typename DoubleHashedTable::Iterator<U>::mem_iterator it_m,
                                              typename DoubleHashedTable::Iterator<U>::mem_iterator it_m_end)
        : it_m(it_m)
        , it_m_end(it_m_end)
{
    while (this->it_m != this->it_m_end && this->it_m->first == ROW_FREE) {
        ++(*this);
    }
}

template<typename K, typename V>
template<typename U>
typename DoubleHashedTable<K, V>::template Iterator<U>& DoubleHashedTable<K, V>::Iterator<U>::operator++()
{
    do {
        ++it_m;
    } while (it_m != it_m_end && it_m->first == ROW_FREE);

    return *this;
}

template<typename K, typename V>
template<typename U>
bool DoubleHashedTable<K, V>::Iterator<U>::operator==(const typename DoubleHashedTable<K, V>::template Iterator<U> &other) const
{
    return it_m == other.it_m;
}

template<typename K, typename V>
template<typename U>
bool DoubleHashedTable<K, V>::Iterator<U>::operator!=(const typename DoubleHashedTable<K, V>::template Iterator<U> &other) const
{
    return !(*this == other);
}

template<typename K, typename V>
template<typename U>
U& DoubleHashedTable<K, V>::Iterator<U>::operator*() const
{
    return it_m->second;
}

template<typename K, typename V>
template<typename U>
U* DoubleHashedTable<K, V>::Iterator<U>::operator->() const
{
    return &it_m->second;
}

#endif // __DOUBLEHASHEDTABLE__
