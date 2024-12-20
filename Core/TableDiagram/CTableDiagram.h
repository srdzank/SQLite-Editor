#ifndef TABLEDIAGRAM_H
#define TABLEDIAGRAM_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QList>
#include <QString>
#include <sqlite3.h>

class CustomRectItem : public QGraphicsRectItem {
public:
    explicit CustomRectItem(const QRectF& rect, QGraphicsItem* parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

struct Relationship {
    QGraphicsEllipseItem* start;
    QGraphicsEllipseItem* end;
    QGraphicsLineItem* line;
};

class TableDiagram : public QGraphicsScene {
    Q_OBJECT

public:
    explicit TableDiagram(sqlite3* db, QObject* parent = nullptr);
    void loadDatabaseSchema();
    void updateConnections(CustomRectItem* table); // Change to public

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    sqlite3* m_db;
    QList<CustomRectItem*> tables;
    QList<Relationship> relationships;

    QGraphicsEllipseItem* selectedPoint = nullptr;
    QGraphicsLineItem* tempLine = nullptr;

    void addTable(const QString& tableName, const QStringList& columns, const QPointF& position);
    QStringList getTableColumns(const QString& tableName);
    void addConnection(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end);
};

#endif // TABLEDIAGRAM_H
