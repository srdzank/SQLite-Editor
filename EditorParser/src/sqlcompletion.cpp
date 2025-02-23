#include "sqlcompletion.h"

SQLCompletion::SQLCompletion() {
    initializeKeywords();
}

void SQLCompletion::initializeKeywords() {
    sqlKeywords << "SELECT" << "FROM" << "WHERE" << "LIKE" << "INSERT" << "UPDATE"
                << "DELETE" << "CREATE" << "DROP" << "ALTER" << "TABLE" << "INDEX";
    // Add more SQL keywords as needed
}

QStringList SQLCompletion::suggest(const std::string& sql, size_t cursorPosition) {
    Lexer lexer(sql);
    std::vector<Token> tokens = lexer.tokenize();

    updateContext(tokens, cursorPosition);

    return getSuggestionsBasedOnContext();
}

void SQLCompletion::updateContext(const std::vector<Token>& tokens, size_t cursorPosition) {
    // Analyze tokens up to cursorPosition to update the current context
    // (e.g., in SELECT clause, in WHERE clause, etc.)
    // This is a simplified version and can be enhanced further.
}

QStringList SQLCompletion::getSuggestionsBasedOnContext() {
    QStringList suggestions;
    // Based on the context, provide relevant suggestions
    // For example, if in SELECT clause, suggest column names and keywords
    // If in FROM clause, suggest table names
    // If in WHERE clause, suggest column names, operators, and keywords

    // Here we are just adding SQL keywords for simplicity.
    // Enhance this based on the context.
    suggestions << sqlKeywords;
    return suggestions;
}
