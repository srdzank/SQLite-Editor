#ifndef SQLCOMPLETION_H
#define SQLCOMPLETION_H

#include "lexer.h"
#include <QStringList>
#include <QCompleter>

class SQLCompletion {
public:
    SQLCompletion();
    QStringList suggest(const std::string& sql, size_t cursorPosition);

private:
    QStringList sqlKeywords;
    QStringList tableNames;
    QStringList columnNames;

    void initializeKeywords();
    void updateContext(const std::vector<Token>& tokens, size_t cursorPosition);
    QStringList getSuggestionsBasedOnContext();
};

#endif // SQLCOMPLETION_H
