#pragma once

#include <QString>

// Represents a foreign key relationship: thisTable.fromColumn -> toTable.toColumn
// Read from PRAGMA foreign_key_list(table).
struct ForeignKey
{
    QString fromTable;
    QString fromColumn;
    QString toTable;
    QString toColumn;
};
