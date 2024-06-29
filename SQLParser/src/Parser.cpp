#include "Parser.h"
#include <stdexcept>

Token Parser::currentToken() {
    return tokens[pos];
}

Token Parser::nextToken() {
    pos++;
    return currentToken();
}

std::shared_ptr<ASTNode> Parser::parse() {
    return parseSelect();
}

std::shared_ptr<ASTNode> Parser::parseSelect() {
    auto node = std::make_shared<SelectNode>();

    if (currentToken().type != TOKEN_SELECT) {
        throw std::runtime_error("Expected SELECT");
    }
    nextToken();

    if (currentToken().type == TOKEN_ASTERISK) {
        node->columns.push_back(std::make_shared<ColumnNode>("*"));
        nextToken();
    }
    else {
        while (currentToken().type == TOKEN_IDENTIFIER) {
            node->columns.push_back(std::make_shared<ColumnNode>(currentToken().value));
            nextToken();
            if (currentToken().type == TOKEN_COMMA) {
                nextToken();
            }
            else {
                break;
            }
        }
    }

    if (currentToken().type != TOKEN_FROM) {
        throw std::runtime_error("Expected FROM");
    }
    nextToken();

    if (currentToken().type != TOKEN_IDENTIFIER) {
        throw std::runtime_error("Expected table name");
    }
    node->table = std::make_shared<TableNode>(currentToken().value);
    nextToken();

    if (currentToken().type == TOKEN_WHERE) {
        nextToken();
        node->whereClause = parseCondition();
    }

    if (currentToken().type != TOKEN_EOF) {
 //       throw std::runtime_error("Unexpected token");
    }

    return node;
}

std::shared_ptr<ASTNode> Parser::parseCondition() {
    std::string left = currentToken().value;
    nextToken();
    std::string op = currentToken().value;
    nextToken();
    std::string right = currentToken().value;
    if (currentToken().type == TOKEN_STRING_LITERAL || currentToken().type == TOKEN_NUMBER) {
        right = currentToken().value;
        nextToken();
    }
    else if (currentToken().type == TOKEN_LIKE) {  
        op = "LIKE";  
        nextToken();
        if (currentToken().type != TOKEN_STRING_LITERAL) {
            throw std::runtime_error("Expected string literal after LIKE");
        }
        right = currentToken().value;
        nextToken();
    }
    return std::make_shared<ConditionNode>(left, op, right);
}