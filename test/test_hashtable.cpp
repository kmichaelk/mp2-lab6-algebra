#include <gtest.h>
#include "chained_hashtable.h"
#include "doublehashedtable.h"
#include "sparse_doublehashedtable.h"

using TKey = std::string;
using TValue = std::string;

using HashTables_typelist = ::testing::Types<
        ChainedHashTable<TKey, TValue>,
        DoubleHashedTable<TKey, TValue>,
        SparseDoubleHashedTable<TKey, TValue>
        >;
template<class> struct HashTables : ::testing::Test {};
TYPED_TEST_SUITE(HashTables, HashTables_typelist);

TYPED_TEST(HashTables, stress_test)
{
    TypeParam table;
    // -- initial_capacity = 3

    table["key1"] = "value1";
    table["key2"] = "value2";
    table["key3"] = "value3";
    //
    table["key4"] = "value4";
    table["key5"] = "value5";

    EXPECT_EQ("value1", table["key1"]);
    EXPECT_EQ("value2", table["key2"]);
    EXPECT_EQ("value3", table["key3"]);
    //
    EXPECT_EQ("value4", table["key4"]);
    EXPECT_EQ("value5", table["key5"]);
}
