#ifndef SQLPARSERAPI_H
#define SQLPARSERAPI_H

#include <memory>
#include <string>
#include <vector>
#include <sqlite3.h>
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "visitor.h"

#ifdef SQLPARSERLIB_EXPORTS
#define SQLPARSERLIB_API __declspec(dllexport)
#else
#define SQLPARSERLIB_API __declspec(dllimport)
#endif

struct SQLResult {
    std::vector<std::vector<std::string>> data;
};

struct SQLSuggestions {
    std::vector<std::string> data;
};

extern "C" {
    SQLPARSERLIB_API int openDatabase(const char* filename, sqlite3** db);
    SQLPARSERLIB_API void closeDatabase(sqlite3* db);
    SQLPARSERLIB_API std::shared_ptr<ASTNode> parseSQL(const std::string& sql);
    SQLPARSERLIB_API void* executeAST(std::shared_ptr<ASTNode> ast, sqlite3* db);
    SQLPARSERLIB_API void freeResults(void* results);
    SQLPARSERLIB_API const char* getResultValue(void* results, size_t row, size_t column);
    SQLPARSERLIB_API size_t getResultRowCount(void* results);
    SQLPARSERLIB_API size_t getResultColumnCount(void* results);
    SQLPARSERLIB_API void* getSuggestions(const char* sql, size_t cursorPosition, sqlite3* db);
    SQLPARSERLIB_API void freeSuggestions(void* suggestions);
    SQLPARSERLIB_API const char* getSuggestionValue(void* suggestions, size_t index);
    SQLPARSERLIB_API size_t getSuggestionsCount(void* suggestions);
}

#endif // SQLPARSERAPI_H
