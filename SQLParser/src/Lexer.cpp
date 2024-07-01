#include "Lexer.h"

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < input.size()) {
        char currentChar = input[pos];
        if (isspace(currentChar)) {
            pos++;
        }
        else if (isdigit(currentChar)) {
            tokens.push_back({ TOKEN_NUMBER, readNumber() });
        }
        else if (isalpha(currentChar)) {
            std::string identifier = readIdentifier();
            tokens.push_back(makeIdentifierToken(identifier));
        }
        else if (currentChar == '\'') {
            tokens.push_back({ TOKEN_STRING_LITERAL, readString() });
        }
        else {
            switch (currentChar) {
            case '*':
                tokens.push_back({ TOKEN_ASTERISK, "*" });
                pos++;
                break;
            case ',':
                tokens.push_back({ TOKEN_COMMA, "," });
                pos++;
                break;
            case '>':
                tokens.push_back({ TOKEN_GREATER_THAN, ">" });
                pos++;
                break;
            case '<':
                tokens.push_back({ TOKEN_LESS_THAN, "<" });
                pos++;
                break;
            case '=':
                tokens.push_back({ TOKEN_EQUALS, "=" });
                pos++;
                break;
            case '(':
                tokens.push_back({ TOKEN_OPEN_PAREN, "(" });
                pos++;
                break;
            case ')':
                tokens.push_back({ TOKEN_CLOSE_PAREN, ")" });
                pos++;
                break;
            default:
                tokens.push_back({ TOKEN_UNKNOWN, std::string(1, currentChar) });
                pos++;
                break;
            }
        }
    }
    tokens.push_back({ TOKEN_EOF, "" });
    return tokens;
}

std::string Lexer::readIdentifier() {
    size_t start = pos;
    while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
        pos++;
    }
    return input.substr(start, pos - start);
}

std::string Lexer::readNumber() {
    size_t start = pos;
    while (pos < input.size() && isdigit(input[pos])) {
        pos++;
    }
    return input.substr(start, pos - start);
}

std::string Lexer::readString() {
    pos++; // Skip opening quote
    size_t start = pos;
    while (pos < input.size() && input[pos] != '\'') {
        pos++;
    }
    std::string str = input.substr(start, pos - start);
    pos++; // Skip closing quote
    return str;
}

Token Lexer::makeIdentifierToken(const std::string& identifier) {
    if (identifier == "SELECT") return { TOKEN_SELECT, identifier };
    if (identifier == "FROM") return { TOKEN_FROM, identifier };
    if (identifier == "WHERE") return { TOKEN_WHERE, identifier };
    if (identifier == "LIKE") return { TOKEN_LIKE, identifier };
    if (identifier == "INSERT") return { TOKEN_INSERT, identifier };
    if (identifier == "INTO") return { TOKEN_INTO, identifier };
    if (identifier == "VALUES") return { TOKEN_VALUES, identifier };
    if (identifier == "UPDATE") return { TOKEN_UPDATE, identifier };
    if (identifier == "DELETE") return { TOKEN_DELETE, identifier };
    if (identifier == "CREATE") return { TOKEN_CREATE, identifier };
    if (identifier == "DROP") return { TOKEN_DROP, identifier };
    if (identifier == "ALTER") return { TOKEN_ALTER, identifier };
    if (identifier == "TABLE") return { TOKEN_TABLE, identifier };
    if (identifier == "JOIN") return { TOKEN_JOIN, identifier };
    if (identifier == "INNER") return { TOKEN_INNER, identifier };
    if (identifier == "LEFT") return { TOKEN_LEFT, identifier };
    if (identifier == "RIGHT") return { TOKEN_RIGHT, identifier };
    if (identifier == "FULL") return { TOKEN_FULL, identifier };
    if (identifier == "OUTER") return { TOKEN_OUTER, identifier };
    if (identifier == "ON") return { TOKEN_ON, identifier };
    if (identifier == "AS") return { TOKEN_AS, identifier };
    if (identifier == "DISTINCT") return { TOKEN_DISTINCT, identifier };
    if (identifier == "IS") return { TOKEN_IS, identifier };
    if (identifier == "BETWEEN") return { TOKEN_BETWEEN, identifier };
    if (identifier == "AND") return { TOKEN_AND, identifier };
    if (identifier == "OR") return { TOKEN_OR, identifier };
    if (identifier == "NOT") return { TOKEN_NOT, identifier };
    if (identifier == "NULL") return { TOKEN_NULL, identifier };
    if (identifier == "DEFAULT") return { TOKEN_DEFAULT, identifier };
    if (identifier == "PRIMARY") return { TOKEN_PRIMARY, identifier };
    if (identifier == "KEY") return { TOKEN_KEY, identifier };
    if (identifier == "FOREIGN") return { TOKEN_FOREIGN, identifier };
    if (identifier == "REFERENCES") return { TOKEN_REFERENCES, identifier };
    if (identifier == "CHECK") return { TOKEN_CHECK, identifier };
    if (identifier == "UNIQUE") return { TOKEN_UNIQUE, identifier };
    if (identifier == "IN") return { TOKEN_IN, identifier };
    if (identifier == "LIMIT") return { TOKEN_LIMIT, identifier };
    if (identifier == "ORDER") return { TOKEN_ORDER, identifier };
    if (identifier == "BY") return { TOKEN_BY, identifier };
    if (identifier == "GROUP") return { TOKEN_GROUP, identifier };
    if (identifier == "HAVING") return { TOKEN_HAVING, identifier };
    if (identifier == "SET") return { TOKEN_SET, identifier };
    return { TOKEN_IDENTIFIER, identifier };
}
