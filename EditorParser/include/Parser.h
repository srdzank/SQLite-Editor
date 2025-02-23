#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    std::shared_ptr<ASTNode> parseSelect();
    std::shared_ptr<ASTNode> parseInsert();
    std::shared_ptr<ASTNode> parseUpdate();
    std::shared_ptr<ASTNode> parseDelete();
    std::shared_ptr<ASTNode> parseCreate();
    std::shared_ptr<ASTNode> parseDrop();
    std::shared_ptr<ASTNode> parseAlter();
    std::shared_ptr<ASTNode> parseJoin();

    Token getNextToken();
    Token peekToken() const;
    bool match(TokenType type);
};

#endif // PARSER_H