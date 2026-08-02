#pragma once

#include <QString>
#include <QStringList>

// Represents a single index on a table, as read from PRAGMA index_list /
// PRAGMA index_info. SQLite's own implicit indexes for PRIMARY KEY /
// UNIQUE constraints (named sqlite_autoindex_...) are filtered out by
// Schema::load - they can't be dropped directly and aren't something a
// user would manage by hand, so only explicitly-created indexes show up.
struct Index
{
    QString name;
    QString table;
    QStringList columns;
    bool isUnique = false;
};
