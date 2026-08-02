#pragma once

#include <QString>
#include <QVector>

#include "Column.h"

// Represents a single table in the schema, along with its columns.
// Foreign keys are stored separately in Schema so relationships between
// tables can be queried without walking every table's column list.
struct Table
{
    QString name;
    QVector<Column> columns;

    const Column* findColumn(const QString& columnName) const
    {
        for (const auto& col : columns) {
            if (col.name == columnName) {
                return &col;
            }
        }
        return nullptr;
    }
};
