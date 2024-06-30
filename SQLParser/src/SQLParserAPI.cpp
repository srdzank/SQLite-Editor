#include "SQLParserAPI.h"
#include "ConcreteVisitor.h"
#include "SQLCompletionEngine.h"
#include <iostream>
#include <sqlite3.h>

SQLPARSERLIB_API int openDatabase(const char* filename, sqlite3** db) {
    int rc = sqlite3_open(filename, db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(*db) << std::endl;
    }
    return rc;
}

SQLPARSERLIB_API void closeDatabase(sqlite3* db) {
    sqlite3_close(db);
}

SQLPARSERLIB_API std::shared_ptr<ASTNode> parseSQL(const std::string& sql) {
    Lexer lexer(sql);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

SQLPARSERLIB_API void* executeAST(std::shared_ptr<ASTNode> ast, sqlite3* db) {
    ConcreteVisitor visitor(db);
    ast->accept(visitor);
    auto results = new SQLResult();
    results->data = visitor.getResults();
    return results;
}

SQLPARSERLIB_API void freeResults(void* results) {
    delete static_cast<SQLResult*>(results);
}

SQLPARSERLIB_API const char* getResultValue(void* results, size_t row, size_t column) {
    SQLResult* res = static_cast<SQLResult*>(results);
    if (row >= res->data.size() || column >= res->data[row].size()) {
        return nullptr;
    }
    return res->data[row][column].c_str();
}

SQLPARSERLIB_API size_t getResultRowCount(void* results) {
    SQLResult* res = static_cast<SQLResult*>(results);
    return res->data.size();
}

SQLPARSERLIB_API size_t getResultColumnCount(void* results) {
    SQLResult* res = static_cast<SQLResult*>(results);
    return res->data.empty() ? 0 : res->data[0].size();
}

SQLPARSERLIB_API void* getSuggestions(const char* sql, size_t cursorPosition, sqlite3* db) {
    SQLCompletionEngine completionEngine(db);
    auto suggestions = new SQLSuggestions();
    suggestions->data = completionEngine.getSuggestions(sql, cursorPosition);
    return suggestions;
}

SQLPARSERLIB_API void freeSuggestions(void* suggestions) {
    delete static_cast<SQLSuggestions*>(suggestions);
}

SQLPARSERLIB_API const char* getSuggestionValue(void* suggestions, size_t index) {
    SQLSuggestions* sug = static_cast<SQLSuggestions*>(suggestions);
    if (index >= sug->data.size()) {
        return nullptr;
    }
    return sug->data[index].c_str();
}

SQLPARSERLIB_API size_t getSuggestionsCount(void* suggestions) {
    SQLSuggestions* sug = static_cast<SQLSuggestions*>(suggestions);
    return sug->data.size();
}
