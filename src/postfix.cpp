#include "postfix.h"
#include <algorithm>
#include <stack>

const std::unordered_map<
        AlgebraicExpression::Operation,
        std::function<Polynomial(const Polynomial &, const Polynomial &)>
> AlgebraicExpression::OPERATIONS = {
        { Operation::Plus, std::plus{} },
        { Operation::Minus, std::minus{} },
        { Operation::Multiply, std::multiplies{} },
        { Operation::Divide, std::divides{} },
};

AlgebraicExpression::AlgebraicExpression(const std::string& infix)
    : tokens(parse(infix))
{
    for (const auto& token : tokens) {
        if (std::holds_alternative<variable_t>(token)) {
            variables.emplace(std::get<variable_t>(token));
        }
    }
}

std::set<AlgebraicExpression::variable_t> AlgebraicExpression::get_variables() const
{
    return variables;
}

template<typename K, typename V>
bool require_keys(const std::unordered_map<K, V>& map, const std::set<K>& keys)
{
    return std::all_of(keys.begin(), keys.end(), [&map](K key) { return map.find(key) != map.end(); });
}

AlgebraicExpression::value_t AlgebraicExpression::evaluate(const std::unordered_map<variable_t, value_t>& values) const
{
    if (!require_keys(values, variables))
        throw std::invalid_argument("Not all variables values are present");

    std::stack<value_t> stack;
    for (const auto& lexeme : tokens)
    {
        if (const auto operation(std::get_if<Operation>(&lexeme)); operation) {
            auto rhs = stack.top(); stack.pop();
            auto lhs = stack.top(); stack.pop();
            stack.push(OPERATIONS.at(*operation)(lhs, rhs));
        } else if (std::holds_alternative<value_t>(lexeme)) {
            stack.push(std::get<value_t>(lexeme));
        } else if (std::holds_alternative<variable_t>(lexeme)) {
            stack.push(values.at(std::get<variable_t>(lexeme)));
        } else {
            // should not really happen
#if _DEBUG
            throw std::logic_error("Not implemented");
#endif
        }
    }

    return stack.top();
}
