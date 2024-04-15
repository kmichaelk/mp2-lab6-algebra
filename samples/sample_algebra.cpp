#include <iostream>
#include <optional>
#include <algorithm>
#include <vector>
#include <set>
#include <cassert>

#include "lineartable.h"
#include "ordered_arraytable.h"
#include "chained_hashtable.h"
#include "doublehashedtable.h"
#include "sparse_doublehashedtable.h"
#include "avltree.h"
#include "rbtree.h"

#include "postfix.h"

//////////////////////////////////////////////////////////////////////////

#define EXPAND( x ) x

#define TABLES_EXPAND(macro, ...)                           \
    EXPAND( macro(ArrayTable, __VA_ARGS__) )                \
    EXPAND( macro(LinkedTable, __VA_ARGS__) )               \
    EXPAND( macro(OrderedArrayTable, __VA_ARGS__) )         \
    EXPAND( macro(ChainedHashTable, __VA_ARGS__) )          \
    EXPAND( macro(DoubleHashedTable, __VA_ARGS__) )         \
    EXPAND( macro(AVLTree, __VA_ARGS__) )                   \
    EXPAND( macro(RBTree, __VA_ARGS__) )

//////////////////////////////////////////////////////////////////////////

#define TABLE_T_ENUMERATION(type) type<TK, TV>,
#define TABLE_I_ENUMERATION(type) type<TK, TV>(),

using namespace std;

using TK = std::string;
using TV = Polynomial;

using table_variant_t = std::variant<TABLES_EXPAND(TABLE_T_ENUMERATION) nullptr_t>;

// the sad costs of conscientious refusal from dynamic polymorphism
std::string table_name(table_variant_t& table);
bool table_has(table_variant_t& table, const TK& key);
void table_update(table_variant_t& table, const TK& key, const TV& value);
const TV& table_lookup(const table_variant_t& table, const TK& key);
std::vector<std::reference_wrapper<const TK>> table_keys(const table_variant_t& table);

template <typename ... variant_types>
void fill_variadic_vector(std::vector<std::variant<variant_types...>>& vec)
{ (vec.emplace_back(variant_types{}), ...); }

void trim_string(std::string& str) {
    str.erase(0, str.find_first_not_of("\t\n\v\f\r "));
    str.erase(str.find_last_not_of("\t\n\v\f\r ") + 1);
}
bool is_in_uppercase(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](unsigned char c){ return std::isupper(c); });
}

int main()
{
    std::vector<table_variant_t> tables;
    fill_variadic_vector(tables);
    tables.pop_back();

    size_t table_idx = 0;

    cout << "------------------------------" << endl;
    cout << "     Algebraic Calculator     " << endl;
    cout << "------------------------------" << endl;
    cout << endl;

    cout << " ? > dump table          - print active table" << endl;
    cout << " ? > select table [idx]  - select active table" << endl;
    cout << " ? > VAR = [polynomial]  - assign polynomial to variable" << endl;
    cout << " ? > [expression]        - evaluate expression" << endl;
    cout << " *** Variables should be in upper case" << endl;
    cout << endl;

    for (size_t i = 0; i < tables.size(); i++) {
        cout << " #" << (i + 1) << ": " << table_name(tables[i]) << endl;
    }
    cout << endl;

    string input;
    size_t eq_pos;
    std::string var_buf, var_value_buf;
    std::optional<Polynomial> polynomial_buf;
    std::optional<AlgebraicExpression> expression;
    while (true) {
        cout << ">>> ";
        getline(cin, input);

        if ("exit" == input) {
            break;
        } else if ("dump table" == input) {
            cout << " @ " << table_name(tables[table_idx]) << " (ID#" << (table_idx + 1) << ")" << endl;
            for (const auto& key: table_keys(tables[table_idx])) {
                cout << "   -- '" << key.get() << "' -> " << "'" << table_lookup(tables[table_idx], key) << "'" << endl;
            }
        } else if (input.rfind("select table", 0) == 0) {
            int new_idx;
            if (!sscanf_s(input.c_str(), "select table %d", &new_idx)) {
                cerr << "Invalid query" << endl;
                continue;
            }
            new_idx--;
            if (new_idx < 0 || new_idx >= tables.size()) {
                cerr << "Table not found" << endl;
                continue;
            }
            table_idx = new_idx;
            cout << "Active table has been set to " << table_name(tables[table_idx]) << endl;
        } else {
            eq_pos = input.find('=');
            if (eq_pos < input.length() && '=' == input[eq_pos]) {
                var_buf = input.substr(0, eq_pos);
                trim_string(var_buf);

                if (!is_in_uppercase(var_buf)) {
                    cerr << "Variable name should be in uppercase" << endl;
                    continue;
                }

                var_value_buf = input.substr(eq_pos + 1);
                trim_string(var_value_buf);

                try {
                    polynomial_buf = Polynomial(var_value_buf);
                } catch (const std::exception& ex) {
                    cerr << "Malformed polynomial value: " << ex.what() << endl;
                    continue;
                }

                for (auto& table : tables) {
                    table_update(table, var_buf, *polynomial_buf);
                }

                cout << "Variable value has been saved" << endl;
                continue;
            }

            try {
                expression = AlgebraicExpression(input);
            } catch (const std::exception& ex) {
                cerr << "Malformed expression: " << ex.what() << endl;
                continue;
            }

            std::unordered_map<std::string, Polynomial> vars;
            for (const auto& key: table_keys(tables[table_idx])) {
                vars[key] = table_lookup(tables[table_idx], key);
            }

            try {
                polynomial_buf = expression->evaluate(vars);
            } catch (const std::exception& ex) {
                cerr << "Failed to evaluate: " << ex.what() << endl;
                continue;
            }

            cout << *polynomial_buf << endl;
        }
    }

    return EXIT_SUCCESS;
}

/////////////////////////   HELL   /////////////////////////

#define TABLE_RET_NAME(type, table)                       \
    if (std::holds_alternative<type<TK, TV>>(table)) {    \
        return #type;                                     \
    }

#define TABLE_HAS(type, table, key)                       \
    if (std::holds_alternative<type<TK, TV>>(table)) {    \
        return std::get<type<TK, TV>>(table).has(key);    \
    }

#define TABLE_UPDATE(type, table, key, value)            \
    if (std::holds_alternative<type<TK, TV>>(table)) {   \
        std::get<type<TK, TV>>(table)[key] = value;      \
        return;                                          \
    }

#define TABLE_LOOKUP(type, table, key)                   \
    if (std::holds_alternative<type<TK, TV>>(table)) {   \
        return std::get<type<TK, TV>>(table).at(key);    \
    }

#define TABLE_KEY_SET(type, table, key_set)              \
    if (std::holds_alternative<type<TK, TV>>(table)) {   \
        const auto& t = std::get<type<TK, TV>>(table);   \
        for (const auto& item : t) {                     \
            (key_set).emplace_back(item.key);            \
        }                                                \
        return key_set;                                  \
    }

std::string table_name(table_variant_t& table)
{
    TABLES_EXPAND(TABLE_RET_NAME, table);
    assert(false);
}
bool table_has(table_variant_t& table, const TK& key)
{
    TABLES_EXPAND(TABLE_HAS, table, key);
    assert(false);
}
void table_update(table_variant_t& table, const TK& key, const TV& value)
{
    TABLES_EXPAND(TABLE_UPDATE, table, key, value);
    assert(false);
}
const TV& table_lookup(const table_variant_t& table, const TK& key)
{
    TABLES_EXPAND(TABLE_LOOKUP, table, key);
    assert(false);
}
std::vector<std::reference_wrapper<const TK>> table_keys(const table_variant_t& table)
{
    std::vector<std::reference_wrapper<const TK>> key_set;
    TABLES_EXPAND(TABLE_KEY_SET, table, key_set);
    assert(false);
}