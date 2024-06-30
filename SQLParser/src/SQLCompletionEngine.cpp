#include "SQLCompletionEngine.h"
#include <algorithm>
#include <iostream>

SQLCompletionEngine::SQLCompletionEngine(sqlite3* db) : db(db) {
    sqlKeywords = {
        "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE", "CREATE", "DROP", "ALTER",
        "TABLE", "INDEX", "VIEW", "TRIGGER", "IF", "EXISTS", "NOT", "NULL", "DEFAULT",
        "PRIMARY", "KEY", "FOREIGN", "REFERENCES", "CHECK", "UNIQUE", "AND", "OR", "IN",
        "LIKE", "LIMIT", "ORDER", "BY", "GROUP", "HAVING", "JOIN", "INNER", "LEFT", "RIGHT",
        "FULL", "OUTER", "ON", "AS", "DISTINCT", "IS", "BETWEEN"
    };
}

std::vector<std::string> SQLCompletionEngine::getSuggestions(const std::string& sql, size_t cursorPosition) {
    Lexer lexer(sql);
    std::vector<Token> tokens = lexer.tokenize();

    std::string context = getCurrentContext(tokens, cursorPosition);

    std::vector<std::string> suggestions;
    if (context == "SELECT" || context == "WHERE" || context == "ORDER" || context == "GROUP") {
        suggestions = sqlKeywords;
        std::vector<std::string> tables = getTableNames();
        for (const auto& table : tables) {
            std::vector<std::string> columns = getColumnNames(table);
            suggestions.insert(suggestions.end(), columns.begin(), columns.end());
        }
    }
    else if (context == "FROM") {
        suggestions = getTableNames();
    }
    else {
        suggestions = sqlKeywords;
    }

    std::sort(suggestions.begin(), suggestions.end());
    suggestions.erase(std::unique(suggestions.begin(), suggestions.end()), suggestions.end());

    return suggestions;
}

std::vector<std::string> SQLCompletionEngine::getTableNames() {
    std::vector<std::string> tableNames;
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table';";
    auto results = executeQuery(sql);
    for (const auto& row : results) {
        if (!row.empty()) {
            tableNames.push_back(row[0]);
        }
    }
    return tableNames;
}

std::vector<std::string> SQLCompletionEngine::getColumnNames(const std::string& tableName) {
    std::vector<std::string> columnNames;
    std::string sql = "PRAGMA table_info(" + tableName + ");";
    auto results = executeQuery(sql);
    for (const auto& row : results) {
        if (!row.empty()) {
            columnNames.push_back(row[1]);
        }
    }
    return columnNames;
}

std::string SQLCompletionEngine::getCurrentContext(const std::vector<Token>& tokens, size_t cursorPosition) {
    std::string context;
    size_t position = 0;
    for (const auto& token : tokens) {
        position += token.value.length();
        if (token.type == TOKEN_SELECT || token.type == TOKEN_FROM || token.type == TOKEN_WHERE ||
            token.type == TOKEN_ORDER || token.type == TOKEN_GROUP) {
            context = token.value;
        }
        if (position >= cursorPosition) {
            break;
        }
    }
    return context;
}

std::vector<std::vector<std::string>> SQLCompletionEngine::executeQuery(const std::string& sql) {
    sqlite3_stmt* stmt;
    std::vector<std::vector<std::string>> result;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return result;
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int col = 0; col < cols; col++) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
            row.push_back(text ? text : "NULL");
        }
        result.push_back(row);
    }

    sqlite3_finalize(stmt);
    return result;
}
