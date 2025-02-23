#ifndef SQLPARSERAPI_H
#define SQLPARSERAPI_H

#include <memory>
#include <string>
#include <vector>
#include "../sqlite3/include/sqlite3.h"
#include "Parser.h"


// Forward declare ASTNode
struct ASTNode;

struct SQLResult {
    std::vector<std::vector<std::string>> data;
};

struct SQLSuggestions {
    std::vector<std::string> data;
};

#ifdef __cplusplus
extern "C" {
#endif

    // Define C types
    typedef struct ASTNode ASTNode;
    typedef struct SQLResult SQLResult;
    typedef struct SQLSuggestions SQLSuggestions;

    // Define API export/import macro for Windows
#define SQLPARSERLIB_API __declspec(dllexport)

    SQLPARSERLIB_API int openDatabase(const char* filename, sqlite3** db);
    SQLPARSERLIB_API void closeDatabase(sqlite3* db);

    SQLPARSERLIB_API ASTNode* parseSQL(const char* sql);
    SQLPARSERLIB_API void freeASTNode(ASTNode* node);

    SQLPARSERLIB_API void* executeAST(ASTNode* ast, sqlite3* db);
    SQLPARSERLIB_API void freeResults(void* results);

    SQLPARSERLIB_API const char* getResultValue(void* results, size_t row, size_t column);
    SQLPARSERLIB_API size_t getResultRowCount(void* results);
    SQLPARSERLIB_API size_t getResultColumnCount(void* results);

    SQLPARSERLIB_API void* getSuggestions(const char* sql, size_t cursorPosition, sqlite3* db);
    SQLPARSERLIB_API void freeSuggestions(void* suggestions);

    SQLPARSERLIB_API const char* getSuggestionValue(void* suggestions, size_t index);
    SQLPARSERLIB_API size_t getSuggestionsCount(void* suggestions);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// C++ specific API declaration
std::shared_ptr<ASTNode> parseSQLCPP(const std::string& sql);
#endif

#endif // SQLPARSERAPI_H
