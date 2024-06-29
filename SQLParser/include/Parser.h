#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "Lexer.h"
#include "AST.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    std::shared_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token currentToken();
    Token nextToken();
    std::shared_ptr<ASTNode> parseSelect();
    std::shared_ptr<ASTNode> parseCondition();
};

#endif // PARSER_H
