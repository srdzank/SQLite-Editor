#ifndef SQLCOMPLETIONENGINE_H
#define SQLCOMPLETIONENGINE_H

#include <vector>
#include <string>
#include <sqlite3.h>
#include "lexer.h"
#include "parser.h"
#include "AST.h"

class SQLCompletionEngine {
public:
    explicit SQLCompletionEngine(sqlite3* db);
    std::vector<std::string> getSuggestions(const std::string& sql, size_t cursorPosition);

private:
    sqlite3* db;
    std::vector<std::string> sqlKeywords;
    std::vector<std::string> getTableNames();
    std::vector<std::string> getColumnNames(const std::string& tableName);
    std::string getCurrentContext(const std::vector<Token>& tokens, size_t cursorPosition);
    std::vector<std::vector<std::string>> executeQuery(const std::string& sql); // Add executeQuery as a private method
};

#endif // SQLCOMPLETIONENGINE_H
