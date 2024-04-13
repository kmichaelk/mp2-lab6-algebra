#include <gtest.h>
#include <algorithm>

#include "lineartable.h"
#include "ordered_arraytable.h"
#include "chained_hashtable.h"
#include "doublehashedtable.h"
#include "sparse_doublehashedtable.h"
#include "avltree.h"
#include "rbtree.h"

using TKey = std::string;
using TValue = std::string;

using Tables_typelist = ::testing::Types<
        ArrayTable<TKey, TValue>,
        LinkedTable<TKey, TValue>,
        //
        OrderedArrayTable<TKey, TValue>,
        //
        ChainedHashTable<TKey, TValue>,
        DoubleHashedTable<TKey, TValue>,
        SparseDoubleHashedTable<TKey, TValue>,
        //
        AVLTree<TKey, TValue>,
        RBTree<TKey, TValue>
        >;
template<class> struct Tables : ::testing::Test {};
TYPED_TEST_SUITE(Tables, Tables_typelist);

TYPED_TEST(Tables, can_add)
{
    TypeParam table;
    table["test"] = "example";

    ASSERT_EQ(1, table.size());
    ASSERT_EQ(true, table.has("test"));
    EXPECT_EQ("example", table["test"]);
}

TYPED_TEST(Tables, can_update_value)
{
    TypeParam table;
    table["test"] = "example";

    ASSERT_EQ(true, table.has("test"));
    EXPECT_EQ("example", table["test"]);

    table["test"] = "new";

    ASSERT_EQ(true, table.has("test"));
    EXPECT_EQ("new", table["test"]);
}

TYPED_TEST(Tables, can_erase)
{
    TypeParam table;
    table["test"] = "example";

    ASSERT_EQ(true, table.has("test"));
    ASSERT_EQ(1, table.size());

    table.erase("test");

    EXPECT_EQ(false, table.has("test"));
    EXPECT_EQ(0, table.size());
}

TYPED_TEST(Tables, can_clear)
{
    TypeParam table;
    table["key1"] = "value1";
    table["key2"] = "value2";

    ASSERT_EQ(true, table.has("key1"));
    ASSERT_EQ(true, table.has("key2"));
    ASSERT_EQ(2, table.size());

    table.clear();

    EXPECT_EQ(false, table.has("key1"));
    EXPECT_EQ(false, table.has("key2"));
    EXPECT_EQ(0, table.size());

    table["key1"] = "value1";
    table["key2"] = "value2";

    EXPECT_EQ(true, table.has("key1"));
    EXPECT_EQ(true, table.has("key2"));
    EXPECT_EQ(2, table.size());
}

TYPED_TEST(Tables, init_list_ctor)
{
    TypeParam table = {
            { "test", "example" },
            { "key1", "value1" },
            { "key2", "value2" },
            { "key3", "value3" },
    };

    ASSERT_EQ(4, table.size());

    EXPECT_EQ("example", table.at("test"));
    EXPECT_EQ("value1", table.at("key1"));
    EXPECT_EQ("value2", table.at("key2"));
    EXPECT_EQ("value3", table.at("key3"));
}

TYPED_TEST(Tables, table_without_elements_is_empty)
{
    TypeParam table;
    EXPECT_EQ(0, table.size());
    EXPECT_EQ(true, table.empty());
}

TYPED_TEST(Tables, table_does_not_contain_non_existent_key)
{
    TypeParam table;
    table["test"] = "example";

    EXPECT_EQ(true, table.has("test"));
    EXPECT_EQ(false, table.has("example"));
}

TYPED_TEST(Tables, iterator)
{
    const std::initializer_list<std::pair<std::string, std::string>> records = {
            { "test", "example" },
            { "key1", "value1" },
            { "key2", "value2" },
            { "key3", "value3" },
    };

    TypeParam table = records;

    ASSERT_EQ(records.size(), table.size());

    std::set<decltype(records)::value_type::first_type> key_set;
    std::transform(records.begin(), records.end(),
                   std::inserter(key_set, key_set.end()),
                   [](const auto& pair){ return pair.first; });
    //
    std::set<decltype(records)::value_type::second_type> value_set;
    std::transform(records.begin(), records.end(),
                   std::inserter(value_set, value_set.end()),
                   [](const auto& pair){ return pair.second; });

    size_t exp_count = 0;
    for (const auto& record : table) {
        EXPECT_EQ(true, key_set.find(record.key) != key_set.end());
        EXPECT_EQ(true, value_set.find(record.value) != value_set.end());
        ++exp_count;
    }

    EXPECT_EQ(records.size(), exp_count);
}
