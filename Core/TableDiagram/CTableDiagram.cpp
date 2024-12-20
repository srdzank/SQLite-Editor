#include "CTableDiagram.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QDebug>

CustomRectItem::CustomRectItem(const QRectF& rect, QGraphicsItem* parent)
    : QGraphicsRectItem(rect, parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant CustomRectItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        if (scene()) {
            auto diagram = dynamic_cast<TableDiagram*>(scene());
            if (diagram) {
                diagram->updateConnections(this);
            }
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

TableDiagram::TableDiagram(sqlite3* db, QObject* parent)
    : QGraphicsScene(parent), m_db(db) {
    loadDatabaseSchema();
}

void TableDiagram::loadDatabaseSchema() {
    clear();
    tables.clear();
    relationships.clear();

    const char* query = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        int xOffset = 50;
        int yOffset = 50;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            QString tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            QStringList columns = getTableColumns(tableName);
            addTable(tableName, columns, QPointF(xOffset, yOffset));
            xOffset += 300;
            if (xOffset > 900) {
                xOffset = 50;
                yOffset += 200;
            }
        }
        sqlite3_finalize(stmt);
    }
}

QStringList TableDiagram::getTableColumns(const QString& tableName) {
    QStringList columns;

    QString query = QString("PRAGMA table_info(%1);").arg(tableName);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, query.toUtf8().constData(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            columns.append(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        }
        sqlite3_finalize(stmt);
    }
    return columns;
}

//void TableDiagram::addTable(const QString& tableName, const QStringList& columns, const QPointF& position) {
//    CustomRectItem* table = new CustomRectItem(QRectF(0, 0, 200, 50 + columns.size() * 20));
//    table->setBrush(Qt::lightGray);
//    table->setPos(position);
//
//    QGraphicsTextItem* tableNameText = addText(tableName, QFont("Arial", 10, QFont::Bold));
//    tableNameText->setDefaultTextColor(Qt::black);
//    tableNameText->setParentItem(table);
//    tableNameText->setPos(5, 5);
//
//    int yOffset = 30;
//    for (const QString& column : columns) {
//        QGraphicsTextItem* columnText = addText(column, QFont("Arial", 8));
//        columnText->setDefaultTextColor(Qt::black);
//        columnText->setParentItem(table);
//        columnText->setPos(20, yOffset);
//
//        QGraphicsEllipseItem* connectionPoint = addEllipse(0, 0, 10, 10, QPen(Qt::black), QBrush(Qt::green));
//        connectionPoint->setParentItem(table);
//        connectionPoint->setPos(5, yOffset + 3);
//
//        yOffset += 20;
//    }
//
//    tables.append(table);
//    addItem(table);
//}


void TableDiagram::addTable(const QString& tableName, const QStringList& columns, const QPointF& position) {
    CustomRectItem* table = new CustomRectItem(QRectF(0, 0, 200, 50 + columns.size() * 20));
    table->setBrush(Qt::lightGray);
    table->setPos(position);

    // Assign table name to the table rect item
    table->setData(0, tableName); // Key 0 for table name

    QGraphicsTextItem* tableNameText = addText(tableName, QFont("Arial", 10, QFont::Bold));
    tableNameText->setDefaultTextColor(Qt::black);
    tableNameText->setParentItem(table);
    tableNameText->setPos(5, 5);

    int yOffset = 30;
    for (const QString& column : columns) {
        QGraphicsTextItem* columnText = addText(column, QFont("Arial", 8));
        columnText->setDefaultTextColor(Qt::black);
        columnText->setParentItem(table);
        columnText->setPos(20, yOffset);

        QGraphicsEllipseItem* connectionPoint = addEllipse(0, 0, 10, 10, QPen(Qt::black), QBrush(Qt::green));
        connectionPoint->setParentItem(table);
        connectionPoint->setPos(5, yOffset + 3);

        // Assign column name to the connection point
        connectionPoint->setData(0, column); // Key 0 for column name

        yOffset += 20;
    }

    tables.append(table);
    addItem(table);
}

void TableDiagram::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem* clickedItem = itemAt(event->scenePos(), QTransform());
    if (clickedItem && dynamic_cast<QGraphicsEllipseItem*>(clickedItem)) {
        QGraphicsEllipseItem* clickedPoint = dynamic_cast<QGraphicsEllipseItem*>(clickedItem);

        if (!selectedPoint) {
            selectedPoint = clickedPoint;
            tempLine = addLine(QLineF(clickedPoint->scenePos(), event->scenePos()), QPen(Qt::blue, 2));
        } else if (selectedPoint != clickedPoint) {
            addConnection(selectedPoint, clickedPoint);

            delete tempLine;
            tempLine = nullptr;
            selectedPoint = nullptr;
        }
    } else if (tempLine) {
        delete tempLine;
        tempLine = nullptr;
        selectedPoint = nullptr;
    }
    QGraphicsScene::mousePressEvent(event);
}

//void TableDiagram::addConnection(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end) {
////    QGraphicsLineItem* connectionLine = addLine(QLineF(start->scenePos(), end->scenePos()), QPen(Qt::green, 2));
//    QGraphicsLineItem* connectionLine = addLine(QLineF(start->sceneBoundingRect().center(), end->sceneBoundingRect().center()), QPen(Qt::green, 2));
//
//    
//    relationships.append({start, end, connectionLine});
//}

void TableDiagram::addConnection(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end) {
    if (!start || !end || !start->parentItem() || !end->parentItem()) {
        qDebug() << "Invalid connection points!";
        return;
    }

    // Debug table and column names
    QString startTable = start->parentItem()->data(0).toString(); // Table name
    QString startColumn = start->data(0).toString();             // Column name
    QString endTable = end->parentItem()->data(0).toString();    // Table name
    QString endColumn = end->data(0).toString();                 // Column name

    qDebug() << "Creating connection between:";
    qDebug() << "Start Table:" << startTable << ", Start Column:" << startColumn;
    qDebug() << "End Table:" << endTable << ", End Column:" << endColumn;

    if (startTable.isEmpty() || startColumn.isEmpty() || endTable.isEmpty() || endColumn.isEmpty()) {
        qDebug() << "Error: Missing table or column name!";
        return;
    }

    // Create the connection line
    QGraphicsLineItem* connectionLine = addLine(QLineF(start->sceneBoundingRect().center(), end->sceneBoundingRect().center()), QPen(Qt::green, 2));
    relationships.append({ start, end, connectionLine });
}


void TableDiagram::updateConnections(CustomRectItem* table) {
    for (auto& rel : relationships) {
        if (rel.start->parentItem() == table || rel.end->parentItem() == table) {
            //rel.line->setLine(QLineF(rel.start->scenePos(), rel.end->scenePos()));
            rel.line->setLine(QLineF(rel.start->sceneBoundingRect().center(), rel.end->sceneBoundingRect().center()));
        }
    }
}

QString TableDiagram::generateSelectSQL() {
    if (relationships.isEmpty()) {
        return "No relationships defined for generating SQL.";
    }

    QStringList selectFields;
    QStringList joinClauses;
    QString baseTable;

    for (const auto& rel : relationships) {
        auto start = rel.start;
        auto end = rel.end;

        // Retrieve table and column names
        QString startTable = start->parentItem()->data(0).toString(); // Table name
        QString startColumn = start->data(0).toString();             // Column name
        QString endTable = end->parentItem()->data(0).toString();    // Table name
        QString endColumn = end->data(0).toString();                 // Column name

        // Debug relationships
        qDebug() << "Processing relationship:";
        qDebug() << "Start Table:" << startTable << ", Start Column:" << startColumn;
        qDebug() << "End Table:" << endTable << ", End Column:" << endColumn;

        // Validate relationship data
        if (startTable.isEmpty() || startColumn.isEmpty() || endTable.isEmpty() || endColumn.isEmpty()) {
            qDebug() << "Invalid relationship encountered!";
            continue;
        }

        // Add to SELECT clause
        if (!selectFields.contains(startTable + "." + startColumn)) {
            selectFields.append(startTable + "." + startColumn);
        }
        if (!selectFields.contains(endTable + "." + endColumn)) {
            selectFields.append(endTable + "." + endColumn);
        }

        // Add to JOIN clause
        joinClauses.append(QString("JOIN %1 ON %2.%3 = %4.%5")
            .arg(endTable)
            .arg(startTable)
            .arg(startColumn)
            .arg(endTable)
            .arg(endColumn));

        // Set base table
        if (baseTable.isEmpty()) {
            baseTable = startTable;
        }
    }

    // Construct SQL query
    return QString("SELECT %1\nFROM %2\n%3;")
        .arg(selectFields.join(", "))
        .arg(baseTable)
        .arg(joinClauses.join("\n"));
}

