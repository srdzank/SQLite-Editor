#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

std::shared_ptr<ASTNode> Parser::parse() {
    Token token = peekToken();
    switch (token.type) {
    case TOKEN_SELECT:
        return parseSelect();
    case TOKEN_INSERT:
        return parseInsert();
    case TOKEN_UPDATE:
        return parseUpdate();
    case TOKEN_DELETE:
        return parseDelete();
    case TOKEN_CREATE:
        return parseCreate();
    case TOKEN_DROP:
        return parseDrop();
    case TOKEN_ALTER:
        return parseAlter();
    case TOKEN_JOIN:
        return parseJoin();
    default:
        throw std::runtime_error("Unsupported SQL statement");
    }
}

std::shared_ptr<ASTNode> Parser::parseSelect() {
    auto selectNode = std::make_shared<SelectNode>();
    match(TOKEN_SELECT);
    while (!match(TOKEN_FROM)) {
        selectNode->columns.push_back(std::make_shared<ColumnNode>(getNextToken().value));
        if (!match(TOKEN_COMMA)) break;
    }
    selectNode->table = std::make_shared<TableNode>(getNextToken().value);
    if (match(TOKEN_WHERE)) {
        selectNode->whereClause = std::make_shared<ConditionNode>(
            getNextToken().value, getNextToken().value, getNextToken().value);
    }
    return selectNode;
}

std::shared_ptr<ASTNode> Parser::parseInsert() {
    auto insertNode = std::make_shared<InsertNode>();
    match(TOKEN_INSERT);
    match(TOKEN_INTO);
    insertNode->table = std::make_shared<TableNode>(getNextToken().value);
    match(TOKEN_OPEN_PAREN);
    while (!match(TOKEN_CLOSE_PAREN)) {
        insertNode->columns.push_back(std::make_shared<ColumnNode>(getNextToken().value));
        if (!match(TOKEN_COMMA)) break;
    }
    match(TOKEN_VALUES);
    match(TOKEN_OPEN_PAREN);
    while (!match(TOKEN_CLOSE_PAREN)) {
        insertNode->values.push_back(getNextToken().value);
        if (!match(TOKEN_COMMA)) break;
    }
    return insertNode;
}

std::shared_ptr<ASTNode> Parser::parseUpdate() {
    auto updateNode = std::make_shared<UpdateNode>();
    match(TOKEN_UPDATE);
    updateNode->table = std::make_shared<TableNode>(getNextToken().value);
    match(TOKEN_SET);
    while (!match(TOKEN_WHERE)) {
        updateNode->columns.push_back(std::make_shared<ColumnNode>(getNextToken().value));
        match(TOKEN_EQUALS);
        updateNode->values.push_back(getNextToken().value);
        if (!match(TOKEN_COMMA)) break;
    }
    if (match(TOKEN_WHERE)) {
        updateNode->whereClause = std::make_shared<ConditionNode>(
            getNextToken().value, getNextToken().value, getNextToken().value);
    }
    return updateNode;
}

std::shared_ptr<ASTNode> Parser::parseDelete() {
    auto deleteNode = std::make_shared<DeleteNode>();
    match(TOKEN_DELETE);
    match(TOKEN_FROM);
    deleteNode->table = std::make_shared<TableNode>(getNextToken().value);
    if (match(TOKEN_WHERE)) {
        deleteNode->whereClause = std::make_shared<ConditionNode>(
            getNextToken().value, getNextToken().value, getNextToken().value);
    }
    return deleteNode;
}

std::shared_ptr<ASTNode> Parser::parseCreate() {
    auto createNode = std::make_shared<CreateNode>();
    match(TOKEN_CREATE);
    match(TOKEN_TABLE);
    createNode->table = std::make_shared<TableNode>(getNextToken().value);
    return createNode;
}

std::shared_ptr<ASTNode> Parser::parseDrop() {
    auto dropNode = std::make_shared<DropNode>();
    match(TOKEN_DROP);
    match(TOKEN_TABLE);
    dropNode->table = std::make_shared<TableNode>(getNextToken().value);
    return dropNode;
}

std::shared_ptr<ASTNode> Parser::parseAlter() {
    auto alterNode = std::make_shared<AlterNode>();
    match(TOKEN_ALTER);
    match(TOKEN_TABLE);
    alterNode->table = std::make_shared<TableNode>(getNextToken().value);
    return alterNode;
}

std::shared_ptr<ASTNode> Parser::parseJoin() {
    auto joinNode = std::make_shared<JoinNode>();
    joinNode->joinType = getNextToken().value; // INNER, LEFT, RIGHT, etc.
    match(TOKEN_JOIN);
    joinNode->leftTable = std::make_shared<TableNode>(getNextToken().value);
    match(TOKEN_ON);
    joinNode->rightTable = std::make_shared<TableNode>(getNextToken().value);
    joinNode->condition = std::make_shared<ConditionNode>(
        getNextToken().value, getNextToken().value, getNextToken().value);
    return joinNode;
}

Token Parser::getNextToken() {
    if (pos < tokens.size()) {
        return tokens[pos++];
    }
    return { TOKEN_EOF, "" };
}

Token Parser::peekToken() const {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return { TOKEN_EOF, "" };
}

bool Parser::match(TokenType type) {
    if (peekToken().type == type) {
        getNextToken();
        return true;
    }
    return false;
}
