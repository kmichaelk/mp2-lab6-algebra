#ifndef __ALGEBRA_PARSING_EXCEPTIONS_H__
#define __ALGEBRA_PARSING_EXCEPTIONS_H__

class expression_parse_error : public std::runtime_error
{
public:
    explicit expression_parse_error(const std::string &message) : runtime_error(message) {}
};

#endif // __ALGEBRA_PARSING_EXCEPTIONS_H__
