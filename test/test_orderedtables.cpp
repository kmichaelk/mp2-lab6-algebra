#include <gtest.h>
#include <algorithm>

#include "lineartable.h"
#include "ordered_arraytable.h"
#include "avltree.h"
#include "rbtree.h"

using TKey = int;
using TValue = int;

using OrderedTables_typelist = ::testing::Types<
        OrderedArrayTable<TKey, TValue>,
        AVLTree<TKey, TValue>,
        RBTree<TKey, TValue>
        >;
template<class> struct OrderedTables : ::testing::Test {};
TYPED_TEST_SUITE(OrderedTables, OrderedTables_typelist);

TYPED_TEST(OrderedTables, order_is_kept)
{
    const std::initializer_list<std::pair<int, int>> records = {
            { 2, 0 },
            { 4, 0 },
            { 3, 0 },
            { 1, 0 },
            { 5, 0 },
    };

    TypeParam table = records;

    ASSERT_EQ(records.size(), table.size());

    // set is ordered, so
    std::set<decltype(records)::value_type::first_type> key_set;
    std::transform(records.begin(), records.end(),
                   std::inserter(key_set, key_set.end()),
                   [](const auto& pair){ return pair.first; });

    auto it = key_set.begin();
    for (const auto& record : table) {
        EXPECT_EQ(*it, record.key);
        ++it;
    }

    ASSERT_EQ(0, std::distance(key_set.end(), it));
}
