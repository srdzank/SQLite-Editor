#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

enum TokenType {
    TOKEN_SELECT,
    TOKEN_FROM,
    TOKEN_WHERE,
    TOKEN_LIKE,
    TOKEN_ASTERISK,
    TOKEN_IDENTIFIER,
    TOKEN_COMMA,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN,
    TOKEN_EQUALS,
    TOKEN_STRING_LITERAL,
    TOKEN_NUMBER,
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const std::string& input) : input(input), pos(0) {}
    std::vector<Token> tokenize();

private:
    std::string input;
    size_t pos;

    std::string readIdentifier();
    std::string readNumber();
    std::string readString();
    Token makeIdentifierToken(const std::string& identifier);
};

#endif // LEXER_H
