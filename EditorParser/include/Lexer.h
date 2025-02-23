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
    TOKEN_UNKNOWN,
    TOKEN_INSERT,
    TOKEN_UPDATE,
    TOKEN_DELETE,
    TOKEN_CREATE,
    TOKEN_DROP,
    TOKEN_ALTER,
    TOKEN_TABLE,
    TOKEN_INDEX,
    TOKEN_VIEW,
    TOKEN_TRIGGER,
    TOKEN_IF,
    TOKEN_EXISTS,
    TOKEN_NOT,
    TOKEN_NULL,
    TOKEN_DEFAULT,
    TOKEN_PRIMARY,
    TOKEN_KEY,
    TOKEN_FOREIGN,
    TOKEN_REFERENCES,
    TOKEN_CHECK,
    TOKEN_UNIQUE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_IN,
    TOKEN_LIMIT,
    TOKEN_ORDER,
    TOKEN_BY,
    TOKEN_GROUP,
    TOKEN_HAVING,
    TOKEN_JOIN,
    TOKEN_INNER,
    TOKEN_LEFT,
    TOKEN_RIGHT,
    TOKEN_FULL,
    TOKEN_OUTER,
    TOKEN_ON,
    TOKEN_AS,
    TOKEN_DISTINCT,
    TOKEN_IS,
    TOKEN_BETWEEN,
    TOKEN_VALUES,       // Add this line
    TOKEN_OPEN_PAREN,   // Add this line
    TOKEN_CLOSE_PAREN,  // Add this line
    TOKEN_INTO,         // Add this line
    TOKEN_SET           // Add this line
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
