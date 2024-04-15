#include <iostream> // <-- del

#include <antlr4-runtime.h>
#include <tree/IterativeParseTreeWalker.h>

#include <AlgebraicExpressionLexer.h>
#include <AlgebraicExpressionParser.h>

#include "postfix.h"
#include "parsingexcept.h"

#include "errhandler.h"
#include "converter.h"

using namespace antlr4;
using namespace ANTLRAlgebraicExpressionParser;

//
// > Операции должны выполняться, используя постфиксную форму
// > Использовать ANTLR4
//                                                 ¯\_(ツ)_/¯
//

// todo (?): flatten polynomial operations (+, -) in tree

std::vector<AlgebraicExpression::token_t> AlgebraicExpression::parse(const std::string& infix)
{
    ANTLRInputStream input(infix);
    //
    AlgebraicExpressionLexer lexer(&input);
    //
    CommonTokenStream tokens(&lexer);
    //
    AlgebraicExpressionParser parser(&tokens);
    parser.removeErrorListeners();
    parser.addErrorListener(new ParsingErrorListener());

    AlgebraicExpressionParser::InputContext* tree;

    try {
        tree = parser.input();
    }
    catch (antlr4::ParseCancellationException &e) {
        throw expression_parse_error(e.what());
    }

//    std::cout << tree->toStringTree(&parser, true) << std::endl;

    AlgebraicExpressionPostfixConverter converter;

    tree::IterativeParseTreeWalker walker;
    walker.walk(&converter, tree);

    return converter.result();
}