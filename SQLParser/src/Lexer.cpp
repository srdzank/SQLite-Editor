#include "Lexer.h"
#include <cctype>

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < input.length()) {
        char current = input[pos];
        if (isspace(current)) {
            pos++;
        }
        else if (isalpha(current)) {
            std::string identifier = readIdentifier();
            tokens.push_back(makeIdentifierToken(identifier));
        }
        else if (isdigit(current)) {
            std::string number = readNumber();
            tokens.push_back({ TOKEN_NUMBER, number });
        }
        else if (current == '"') {  
            std::string str = readString();
            tokens.push_back({ TOKEN_STRING_LITERAL, str }); 
        }
        else if (current == '*') {
            tokens.push_back({ TOKEN_ASTERISK, "*" });
            pos++;
        }
        else if (current == ',') {
            tokens.push_back({ TOKEN_COMMA, "," });
            pos++;
        }
        else if (current == '>') {
            tokens.push_back({ TOKEN_GREATER_THAN, ">" });
            pos++;
        }
        else if (current == '<') {
            tokens.push_back({ TOKEN_LESS_THAN, "<" });
            pos++;
        }
        else if (current == '=') {
            tokens.push_back({ TOKEN_EQUALS, "=" });
            pos++;
        }
        else {
            tokens.push_back({ TOKEN_UNKNOWN, std::string(1, current) });
            pos++;
        }
    }
    tokens.push_back({ TOKEN_EOF, "" });
    return tokens;
}

std::string Lexer::readIdentifier() {
    size_t start = pos;
    while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
        pos++;
    }
    return input.substr(start, pos - start);
}

std::string Lexer::readNumber() {
    size_t start = pos;
    while (pos < input.length() && isdigit(input[pos])) {
        pos++;
    }
    return input.substr(start, pos - start);
}

std::string Lexer::readString() {  // <-- Added
    pos++;  // Skip the opening quote
    size_t start = pos;
    while (pos < input.length() && input[pos] != '"') {
        pos++;
    }
    std::string str = input.substr(start, pos - start);
    pos++;  // Skip the closing quote
    str = "'" + str + "'";
    return str;
}

Token Lexer::makeIdentifierToken(const std::string& identifier) {
    if (identifier == "SELECT") {
        return { TOKEN_SELECT, identifier };
    }
    else if (identifier == "FROM") {
        return { TOKEN_FROM, identifier };
    }
    else if (identifier == "WHERE") {
        return { TOKEN_WHERE, identifier };
    }
    else if (identifier == "LIKE") {  
        return { TOKEN_LIKE, identifier }; 
    }
     else {
        return { TOKEN_IDENTIFIER, identifier };
    }
}
