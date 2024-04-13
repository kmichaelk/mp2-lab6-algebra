#ifndef __ORDERED_ARRAYTABLE__
#define __ORDERED_ARRAYTABLE__

#include "lineartable.h"

template<typename K, typename V>
class OrderedArrayTable : public ArrayTable<K, V>
{
public:
    using typename ArrayTable<K, V>::value_type;

    using typename ArrayTable<K, V>::iterator;
    using typename ArrayTable<K, V>::const_iterator;

    using ArrayTable<K, V>::begin;
    using ArrayTable<K, V>::end;

    OrderedArrayTable() = default;
    OrderedArrayTable(std::initializer_list<std::pair<K, V>> init);

protected:
    using ArrayTable<K, V>::list;

    iterator lookup(const K& key);
    const_iterator lookup(const K& key) const;
    value_type& insert(const K& key, const V& value);

private:
    static bool row_key_lb(const Row<K, V>& row, const K& key);

    template<class It>
    static It binary_key_search(It first, It last, const K& key);
};

template<typename K, typename V>
OrderedArrayTable<K, V>::OrderedArrayTable(std::initializer_list<std::pair<K, V>> init)
{
    for (const auto& entry : init) {
        insert(entry.first, entry.second);
    }
}

template<typename K, typename V>
bool OrderedArrayTable<K, V>::row_key_lb(const Row<K, V>& row, const K& key)
{
    return row.key < key;
}

template<typename K, typename V>
template<class It>
It OrderedArrayTable<K, V>::binary_key_search(It first, It last, const K& key)
{
    const auto result = std::lower_bound(first, last, key, row_key_lb);
    if (result != last && result->key == key) {
        return result;
    }
    return last;
}

template<typename K, typename V>
typename OrderedArrayTable<K, V>::iterator OrderedArrayTable<K, V>::lookup(const K& key)
{
    return binary_key_search(begin(), end(), key);
}

template<typename K, typename V>
typename OrderedArrayTable<K, V>::const_iterator OrderedArrayTable<K, V>::lookup(const K& key) const
{
    return binary_key_search(begin(), end(), key);
}

template<typename K, typename V>
typename OrderedArrayTable<K, V>::value_type& OrderedArrayTable<K, V>::insert(const K& key, const V& value)
{
    const auto pos = std::lower_bound(begin(), end(), key, row_key_lb);
    return *list.emplace(pos, key, value);
}

#endif // __ORDERED_ARRAYTABLE__
