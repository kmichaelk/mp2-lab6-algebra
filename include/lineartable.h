#ifndef __LINEARTABLE_H__
#define __LINEARTABLE_H__

#include <cassert>
#include "table_commons.h"

template<typename K, typename V, template<typename> class CList>
class LinearTable
{
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = Row<K, V>;
protected:
    CList<value_type> list;
public:
    using iterator       = typename decltype(list)::iterator;
    using const_iterator = typename decltype(list)::const_iterator;
protected:
    /* virtual */ iterator lookup(const K& key);
    /* virtual */ const_iterator lookup(const K& key) const;
    /* virtual */ value_type& insert(const K& key, const V& value);
public:
    LinearTable() = default;
    LinearTable(std::initializer_list<std::pair<K, V>> init);

    virtual ~LinearTable() = default;

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

    iterator begin();
    iterator end();
    //
    const_iterator begin() const { return cbegin(); };
    const_iterator end() const { return cend(); };
    //
    const_iterator cbegin() const;
    const_iterator cend() const;
};

template<typename K, typename V, template<typename> class CList>
LinearTable<K, V, CList>::LinearTable(std::initializer_list<std::pair<K, V>> init)
{
    for (const auto& entry : init) {
        (*this)[entry.first] = entry.second;
    }
}

template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::iterator LinearTable<K, V, CList>::lookup(const K& key)
{
    return std::find_if(begin(), end(), [&key](const auto& row) {
        return row.key == key;
    });
}

template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::const_iterator LinearTable<K, V, CList>::lookup(const K& key) const
{
    return std::find_if(begin(), end(), [&key](const auto& row) {
        return row.key == key;
    });
}

template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::value_type& LinearTable<K, V, CList>::insert(const K& key, const V& value)
{
    return list.emplace_back(key, value);
}

template<typename K, typename V, template<typename> class CList>
bool LinearTable<K, V, CList>::has(const K& key) const
{
    return lookup(key) != end();
}

template<typename K, typename V, template<typename> class CList>
void LinearTable<K, V, CList>::erase(LinearTable::const_iterator pos)
{
    list.erase(pos);
}

template<typename K, typename V, template<typename> class CList>
void LinearTable<K, V, CList>::erase(const K& key)
{
    const auto it = std::as_const(*this).lookup(key);
    assert(it != std::as_const(*this).end() && "Key not exists");
    erase(it);
}

template<typename K, typename V, template<typename> class CList>
size_t LinearTable<K, V, CList>::size() const
{
    return list.size();
}

template<typename K, typename V, template<typename> class CList>
bool LinearTable<K, V, CList>::empty() const
{
    return list.empty();
}

template<typename K, typename V, template<typename> class CList>
void LinearTable<K, V, CList>::clear()
{
    list.clear();
}

template<typename K, typename V, template<typename> class CList>
V& LinearTable<K, V, CList>::at(const K& key)
{
    return const_cast<V&>(std::as_const(*this).at(key));
}

template<typename K, typename V, template<typename> class CList>
const V& LinearTable<K, V, CList>::at(const K& key) const
{
    const auto it = lookup(key);
    assert(it != end() && "Key not exists");
    return it->value;
}

template<typename K, typename V, template<typename> class CList>
V& LinearTable<K, V, CList>::operator[](const K& key)
{
    auto it = lookup(key);
    if (it != end()) {
        return it->value;
    }
    return insert(key, {}).value;
}

template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::iterator LinearTable<K, V, CList>::begin()
{
    return list.begin();
}
template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::iterator LinearTable<K, V, CList>::end()
{
    return list.end();
}
//
template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::const_iterator LinearTable<K, V, CList>::cbegin() const
{
    return list.cbegin();
}
template<typename K, typename V, template<typename> class CList>
typename LinearTable<K, V, CList>::const_iterator LinearTable<K, V, CList>::cend() const
{
    return list.cend();
}

// region Statically Polymorphic Implementations

#include <vector>
#include "linkedlist.h"

template<typename K, typename T>
using ArrayTable = LinearTable<K, T, std::vector>;

template<typename K, typename T>
using LinkedTable = LinearTable<K, T, LinkedList>;

// C++<17
//namespace _ArrayTable {
//    template<typename T>
//    using DefaultAllocatorVector = std::vector<T>;
//}
//template<typename K, typename T>
//using ArrayTable = LinearTable<K, T, _ArrayTable::DefaultAllocatorVector>;

// endregion

#endif // __LINEARTABLE_H__
