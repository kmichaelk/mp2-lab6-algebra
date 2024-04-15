#ifndef __POSTFIX_H__
#define __POSTFIX_H__

#include <stdexcept>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <vector>
#include <variant>
#include "polynomial.h"

class AlgebraicExpression {
public:
    enum class Operation {
        Plus,
        Minus,
        Multiply,
        Divide
    };
private:
    using value_t = Polynomial;
    using variable_t = std::string;
    using token_t = std::variant<Operation, value_t, variable_t>;

    static const std::unordered_map<Operation, std::function<value_t (const value_t&, const value_t&)>> OPERATIONS;

    std::vector<token_t> tokens;
    std::set<variable_t> variables;

    static std::vector<token_t> parse(const std::string& infix);
public:
    explicit AlgebraicExpression(const std::string& infix);

    [[nodiscard]] std::set<variable_t> get_variables() const;

    [[nodiscard]]
    value_t evaluate(const std::unordered_map<variable_t, value_t>& values = {}) const;
};

#endif // __POSTFIX_H__
