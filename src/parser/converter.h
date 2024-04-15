#ifndef __ALGEBRA_CONVERTER_H__
#define __ALGEBRA_CONVERTER_H__

#include <vector>

#include <AlgebraicExpressionParserBaseListener.h>

#include "postfix.h"

using namespace ANTLRAlgebraicExpressionParser;

class AlgebraicExpressionPostfixConverter : public AlgebraicExpressionParserBaseListener
{
private:
    std::vector<std::variant<AlgebraicExpression::Operation, Polynomial, std::string>> postfix;

    struct {
        Monomial monomial = Monomial(0.0);

        char atom = 0;
        double coefficient = 1;
    } buf;
public:

    auto result() {
        return postfix;
    }

    void exitExpressionSum(AlgebraicExpressionParser::ExpressionSumContext *) override {
        postfix.emplace_back(AlgebraicExpression::Operation::Plus);
    }
    void exitExpressionDiff(AlgebraicExpressionParser::ExpressionDiffContext *) override {
        postfix.emplace_back(AlgebraicExpression::Operation::Minus);
    }
    void exitExpressionMul(AlgebraicExpressionParser::ExpressionMulContext *) override {
        postfix.emplace_back(AlgebraicExpression::Operation::Multiply);
    }
    void exitExpressionDiv(AlgebraicExpressionParser::ExpressionDivContext *) override {
        postfix.emplace_back(AlgebraicExpression::Operation::Divide);
    }

    void enterMonomial(AlgebraicExpressionParser::MonomialContext *) override {
        buf.monomial = Monomial(1.0);
    }

    void exitMonomial(AlgebraicExpressionParser::MonomialContext *ctx) override {
        postfix.emplace_back(buf.monomial);
    }

    void exitVariable(AlgebraicExpressionParser::VariableContext *ctx) override {
        postfix.emplace_back(ctx->getText());
    }

    void exitAtom(AlgebraicExpressionParser::AtomContext *ctx) override {
        buf.atom = 0;
    }

    void exitAtomicVariable(AlgebraicExpressionParser::AtomicVariableContext *ctx) override {
        buf.atom = ctx->getText()[0];
        buf.monomial[buf.atom] = 1;
    }

    void exitCoefficient(AlgebraicExpressionParser::CoefficientContext *ctx) override {
        if (buf.atom) {
            buf.monomial[buf.atom] = std::stoi(ctx->getText());
        } else {
            buf.monomial.set_coefficient(std::stod(ctx->getText()));
        }
    }
};

#endif // __ALGEBRA_CONVERTER_H__
