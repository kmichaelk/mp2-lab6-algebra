#ifndef __PARSER_ANTLR_ERROR_HANDLER__
#define __PARSER_ANTLR_ERROR_HANDLER__

#include <antlr4-runtime.h>

using namespace antlr4;

class ParsingErrorListener : public BaseErrorListener {
public:
    void syntaxError(Recognizer *recognizer, Token * offendingSymbol,
                     size_t line, size_t charPositionInLine, const std::string &msg,
                     std::exception_ptr e) override {
        e = nullptr;

        std::ostringstream oss;
        oss << "pos: " << charPositionInLine << " " << msg;

        throw antlr4::ParseCancellationException(oss.str());
    }
};

#endif // __PARSER_ANTLR_ERROR_HANDLER__
