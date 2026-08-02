#pragma once

#include <QGraphicsRectItem>
#include <QString>

// A single table box on the diagram. Stores the table name as item data
// so DiagramView can identify which table was clicked without a separate
// QObject signal path (QGraphicsItem doesn't support signals directly).
class TableNode : public QGraphicsRectItem
{
public:
    static constexpr qreal kWidth = 110.0;
    static constexpr qreal kHeight = 44.0;

    // isFocus draws the node with the accent (teal) style used for the
    // table currently centered in the diagram; other nodes get the
    // neutral gray style.
    TableNode(const QString& tableName, bool isFocus, QGraphicsItem* parent = nullptr);

    QString tableName() const;
};
