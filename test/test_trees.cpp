#include <gtest.h>
#include <algorithm>

#include "avltree.h"
#include "rbtree.h"

using TKey = std::string;
using TValue = std::string;

using Trees_typelist = ::testing::Types<
        AVLTree<TKey, TValue>,
        RBTree<TKey, TValue>
        >;
template<class> struct Trees : ::testing::Test {};
TYPED_TEST_SUITE(Trees, Trees_typelist);

TYPED_TEST(Trees, can_erase_root)
{
    TypeParam tree;
    tree["test"] = "example";
    tree["t"] = "e";

    ASSERT_EQ(2, tree.size());
    ASSERT_EQ(true, tree.has("test"));
    ASSERT_EQ(true, tree.has("t"));

    tree.erase("test");

    ASSERT_EQ(1, tree.size());
    ASSERT_EQ(false, tree.has("test"));
    ASSERT_EQ(true, tree.has("t"));
}

TYPED_TEST(Trees, can_copy)
{
    TypeParam tree;
    //
    tree["key1"] = "value1";
    tree["key2"] = "value2";
    tree["key3"] = "value3";
    tree["key4"] = "value4";
    tree["key5"] = "value5";

    TypeParam copy = tree;

    ASSERT_EQ(tree.size(), copy.size());

    auto it_copy = tree.begin();

    for (const auto& record : tree) {
        EXPECT_EQ(record, *it_copy);
        ++it_copy;
    }
}
