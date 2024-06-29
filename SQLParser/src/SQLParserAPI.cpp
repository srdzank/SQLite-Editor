#include "SQLParserAPI.h"
#include "ConcreteVisitor.h"
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

SQLPARSERLIB_API std::vector<std::vector<std::string>> executeAST(std::shared_ptr<ASTNode> ast, sqlite3* db) {
    ConcreteVisitor visitor(db);
    ast->accept(visitor);
    return visitor.getResults();
}

SQLPARSERLIB_API std::vector<std::vector<std::string>> executeQuery(sqlite3* db, const std::string& sql) {
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
