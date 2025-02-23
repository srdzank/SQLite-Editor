#include "SQLPARSERAPI.h"
#include "ConcreteVisitor.h"
#include "SQLCompletionEngine.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

#ifdef __cplusplus
extern "C" {
#endif

    // C function to parse SQL and return ASTNode*
    ASTNode* parseSQL(const char* sql) {
#ifdef __cplusplus
        // Call C++ implementation if available
        return parseSQLCPP(sql).get();
#else
        // Implement parsing logic in C (if necessary)
        return nullptr; // Placeholder for C implementation
#endif
    }

    void freeASTNode(ASTNode* node) {
        // This function should be empty as shared_ptr will manage the memory
    }

    void* executeAST(ASTNode* ast, sqlite3* db) {
        std::shared_ptr<ASTNode> astPtr(ast);
        ConcreteVisitor visitor(db);
        astPtr->accept(visitor);
        auto results = new SQLResult();
        results->data = visitor.getResults();
        return results;
    }

    void freeResults(void* results) {
        delete static_cast<SQLResult*>(results);
    }

    const char* getResultValue(void* results, size_t row, size_t column) {
        SQLResult* res = static_cast<SQLResult*>(results);
        if (row >= res->data.size() || column >= res->data[row].size()) {
            return nullptr;
        }
        return res->data[row][column].c_str();
    }

    size_t getResultRowCount(void* results) {
        SQLResult* res = static_cast<SQLResult*>(results);
        return res->data.size();
    }

    size_t getResultColumnCount(void* results) {
        SQLResult* res = static_cast<SQLResult*>(results);
        return res->data.empty() ? 0 : res->data[0].size();
    }

    void* getSuggestions(const char* sql, size_t cursorPosition, sqlite3* db) {
        SQLCompletionEngine completionEngine(db);
        auto suggestions = new SQLSuggestions();
        suggestions->data = completionEngine.getSuggestions(sql, cursorPosition);
        return suggestions;
    }

    void freeSuggestions(void* suggestions) {
        delete static_cast<SQLSuggestions*>(suggestions);
    }

    const char* getSuggestionValue(void* suggestions, size_t index) {
        SQLSuggestions* sug = static_cast<SQLSuggestions*>(suggestions);
        if (index >= sug->data.size()) {
            return nullptr;
        }
        return sug->data[index].c_str();
    }

    size_t getSuggestionsCount(void* suggestions) {
        SQLSuggestions* sug = static_cast<SQLSuggestions*>(suggestions);
        return sug->data.size();
    }

#ifdef __cplusplus
}
#endif

// C++ specific implementation
std::shared_ptr<ASTNode> parseSQLCPP(const std::string& sql) {
    Lexer lexer(sql);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

int openDatabase(const char* filename, sqlite3** db) {
    return sqlite3_open(filename, db);
}

void closeDatabase(sqlite3* db) {
    sqlite3_close(db);
}