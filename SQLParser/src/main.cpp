#include <iostream>
#include <sqlite3.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "Visitor.h"
#include "ConcreteVisitor.h"

int main() {
    std::string input = "SELECT * FROM albums a";

    // Open SQLite database
    sqlite3* db;
    int rc = sqlite3_open("chinook.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    // Tokenize
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    // Parse
    Parser parser(tokens);
    std::shared_ptr<ASTNode> ast = parser.parse();

    // Print AST
    ast->print();

    // Use visitor to execute the AST
    ConcreteVisitor visitor(db);
    ast->accept(visitor);

    // Close SQLite database
    sqlite3_close(db);

    return 0;
}
