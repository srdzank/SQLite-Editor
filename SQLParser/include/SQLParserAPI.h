#ifndef SQLPARSERAPI_H
#define SQLPARSERAPI_H

#include <string>
#include <memory>
#include "AST.h"
#include "Visitor.h"
#include "ConcreteVisitor.h"
#include "Lexer.h"
#include "Parser.h"

#ifdef _WIN32
#ifdef SQLPARSERLIB_EXPORTS
#define SQLPARSERLIB_API __declspec(dllexport)
#else
#define SQLPARSERLIB_API __declspec(dllimport)
#endif
#else
#define SQLPARSERLIB_API
#endif

extern "C" {
    SQLPARSERLIB_API int openDatabase(const char* filename, sqlite3** db);
    SQLPARSERLIB_API void closeDatabase(sqlite3* db);
    SQLPARSERLIB_API std::shared_ptr<ASTNode> parseSQL(const std::string& sql);
    SQLPARSERLIB_API std::vector<std::vector<std::string>>executeAST(std::shared_ptr<ASTNode> ast, sqlite3* db);
}

#endif // SQLPARSERAPI_H
