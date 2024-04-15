#include <gtest.h>
#include <optional>
#include "postfix.h"

TEST(AlgebraicExpression, can_evaluate_atomic_expressions)
{
    std::optional<AlgebraicExpression> expr;
    ASSERT_NO_THROW(expr = AlgebraicExpression("2x^3y^4z^5"));
    EXPECT_EQ(Polynomial("2x^3y^4z^5"), expr->evaluate());
}

TEST(AlgebraicExpression, can_evaluate_simple_expressions)
{
    std::optional<AlgebraicExpression> expr;
    ASSERT_NO_THROW(expr = AlgebraicExpression("2xyz - 3x^2yz"));
    EXPECT_EQ(Polynomial("2xyz - 3x^2yz"), expr->evaluate());
}

TEST(AlgebraicExpression, handles_operations_priorities_properly)
{
    std::optional<AlgebraicExpression> expr;
    //
    ASSERT_NO_THROW(expr = AlgebraicExpression("1 + 2 * 10"));
    EXPECT_EQ(Polynomial("21"), expr->evaluate());
    //
    ASSERT_NO_THROW(expr = AlgebraicExpression("(1 + 2) * 10"));
    EXPECT_EQ(Polynomial("30"), expr->evaluate());
}

TEST(AlgebraicExpression, cant_evaluate_with_variables_missing)
{
    std::optional<AlgebraicExpression> expr;
    ASSERT_NO_THROW(expr = AlgebraicExpression("xyz + VARIABLE"));
    EXPECT_ANY_THROW(expr->evaluate());
}

TEST(AlgebraicExpression, can_evaluate_with_variables)
{
    std::optional<AlgebraicExpression> expr;
    ASSERT_NO_THROW(expr = AlgebraicExpression("xyz + VARIABLE"));
    EXPECT_EQ(Polynomial("10xyz"), expr->evaluate({
        { "VARIABLE", Polynomial("9xyz") }
    }));
}
