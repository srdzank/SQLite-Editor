#include "CERDiagram.h"
#include <QPainterPath>
#include <QDebug>

// TableNodeGroup Implementation
TableNodeGroup::TableNodeGroup(const QString& tableName, ERDiagram* parentDiagram, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent), tableName(tableName), parentDiagram(parentDiagram) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
}


QVariant TableNodeGroup::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        qDebug() << "Table moved. New position:" << value.toPointF();
        if (parentDiagram) {
            parentDiagram->updateRelationshipPositions();
        }
        else {
            qDebug() << "Error: parentDiagram is null!";
        }
    }
    return QGraphicsItemGroup::itemChange(change, value);
}


// ERDiagram Implementation
ERDiagram::ERDiagram(sqlite3* db, QWidget* parent)
    : QGraphicsView(parent), db(db), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

ERDiagram::~ERDiagram() {}

void ERDiagram::generateDiagram() {
    if (!db) {
        qDebug() << "Database not connected.";
        return;
    }

    scene->clear();
    tableNodes.clear();
    relationships.clear();

    const char* tableQuery = "SELECT name FROM sqlite_master WHERE type='table';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, tableQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Failed to query table names: " << sqlite3_errmsg(db);
        return;
    }

    qreal x = 50, y = 50;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QString tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        QVector<QString> fields;
        QString pragmaQuery = QString("PRAGMA table_info(%1);").arg(tableName);
        sqlite3_stmt* attrStmt;

        if (sqlite3_prepare_v2(db, pragmaQuery.toUtf8().constData(), -1, &attrStmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(attrStmt) == SQLITE_ROW) {
                fields.append(reinterpret_cast<const char*>(sqlite3_column_text(attrStmt, 1)));
            }
            sqlite3_finalize(attrStmt);
        }

        createTableNode(tableName, fields, x, y);
        y += 200;
        if (y > 800) {
            y = 50;
            x += 300;
        }
    }
    sqlite3_finalize(stmt);

    for (const QString& tableName : tableNodes.keys()) {
        QString pragmaQuery = QString("PRAGMA foreign_key_list(%1);").arg(tableName);
        if (sqlite3_prepare_v2(db, pragmaQuery.toUtf8().constData(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                QString referencedTable = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                createRelationship(tableName, referencedTable);
            }
            sqlite3_finalize(stmt);
        }
    }
}

void ERDiagram::createTableNode(const QString& tableName, const QVector<QString>& fields, qreal x, qreal y) {
    const qreal rectWidth = 200;
    const qreal baseHeight = 50;
    const qreal fieldHeight = 20;

    qreal rectHeight = baseHeight + fields.size() * fieldHeight;

    TableNodeGroup* group = new TableNodeGroup(tableName, this);

    QGraphicsRectItem* rect = new QGraphicsRectItem(x, y, rectWidth, rectHeight);
    rect->setBrush(Qt::lightGray);
    group->addToGroup(rect);

    QGraphicsTextItem* header = new QGraphicsTextItem(tableName);
    header->setFont(QFont("Arial", 10, QFont::Bold));
    header->setPos(x + 10, y + 10);
    group->addToGroup(header);

    QVector<QGraphicsTextItem*> fieldItems;
    qreal fieldY = y + baseHeight;
    for (const QString& field : fields) {
        QGraphicsTextItem* fieldItem = new QGraphicsTextItem(field);
        fieldItem->setFont(QFont("Arial", 8));
        fieldItem->setPos(x + 10, fieldY);
        group->addToGroup(fieldItem);
        fieldY += fieldHeight;
    }

    scene->addItem(group);
    tableNodes[tableName] = { group, rect, header, fieldItems };
}

void ERDiagram::createRelationship(const QString& fromTable, const QString& toTable) {
    if (!tableNodes.contains(fromTable) || !tableNodes.contains(toTable))
        return;

    QRectF fromRect = tableNodes[fromTable].rect->sceneBoundingRect();
    QRectF toRect = tableNodes[toTable].rect->sceneBoundingRect();

    QPointF fromCenter = fromRect.center();
    QPointF toCenter = toRect.center();

    QPainterPath path(fromCenter);
    path.lineTo(toCenter);

    QGraphicsPathItem* relationship = scene->addPath(path, QPen(Qt::blue, 2));
    relationships.append(RelationshipLine(relationship, fromTable, toTable));
}

void ERDiagram::updateRelationshipPositions() {
    for (auto& relationship : relationships) {
        const QString& fromTable = relationship.fromTable;
        const QString& toTable = relationship.toTable;

        QRectF fromRect = tableNodes[fromTable].rect->sceneBoundingRect();
        QRectF toRect = tableNodes[toTable].rect->sceneBoundingRect();

        QPointF fromCenter = fromRect.center();
        QPointF toCenter = toRect.center();

        QPainterPath path(fromCenter);
        path.lineTo(toCenter);

        relationship.path->setPath(path);
    }
}
