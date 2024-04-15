#ifndef __ALGEBRA_CONVERTER_H__
#define __ALGEBRA_CONVERTER_H__

#include <iostream>
#include <vector>
#include <stack>

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
        std::cout << "MONOMIAL : " << ctx->getText() << std::endl;
        std::cout << " ~~~ parsed: " << buf.monomial << std::endl << std::endl;

        postfix.emplace_back(buf.monomial);
    }

    void exitVariable(AlgebraicExpressionParser::VariableContext *ctx) override {
        postfix.emplace_back(ctx->getText());
    }

    void exitAtom(AlgebraicExpressionParser::AtomContext *ctx) override {
        std::cout << "ATOM : " << ctx->getText() << std::endl;
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

    void exitInput(AlgebraicExpressionParser::InputContext *) override {
        std::cout << "  ---> complete <---" << std::endl;
        std::cout << std::endl;
        //for (const auto &item : postfix) {
        //    std::cout << item << " ";
        //}
        //std::cout << std::endl;
    }
};

#endif // __ALGEBRA_CONVERTER_H__
