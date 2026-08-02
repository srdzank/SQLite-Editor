#pragma once

#include <QString>

// Represents a single column of a table, as read from PRAGMA table_info.
struct Column
{
    QString name;
    QString type;       // e.g. "INTEGER", "NVARCHAR(120)"
    bool isPrimaryKey = false;
    bool isNotNull = false;
};
