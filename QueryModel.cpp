#include "QueryModel.h"

QString QueryModel::toSql() const
{
    if (isEmpty()) {
        return QString();
    }

    QString sql = "SELECT *\nFROM " + m_fromTable;

    for (const auto& join : m_joins) {
        sql += "\nJOIN " + join.table
             + "\n  ON " + join.onRelation.fromTable + "." + join.onRelation.fromColumn
             + " = " + join.onRelation.toTable + "." + join.onRelation.toColumn;
    }

    if (!m_conditions.isEmpty()) {
        sql += "\nWHERE ";
        for (int i = 0; i < m_conditions.size(); ++i) {
            if (i > 0) {
                sql += "\n  AND ";
            }
            const auto& c = m_conditions[i];
            sql += c.column + " " + c.op + " " + c.value;
        }
    }

    sql += ";";
    return sql;
}
