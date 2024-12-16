#ifndef ERDIAGRAM_H
#define ERDIAGRAM_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <sqlite3.h>
#include <QMap>
#include <QVector>
#include <QGraphicsItemGroup>

class ERDiagram;

// Represents a table in the diagram
class TableNodeGroup : public QGraphicsItemGroup {
public:
    explicit TableNodeGroup(const QString& tableName, ERDiagram* parentDiagram, QGraphicsItem* parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QString tableName;
    ERDiagram* parentDiagram;
};

// Represents a relationship line between two tables
class RelationshipLine {
public:
    RelationshipLine(QGraphicsPathItem* path, const QString& fromTable, const QString& toTable)
        : path(path), fromTable(fromTable), toTable(toTable) {
    }

    QGraphicsPathItem* path;
    QString fromTable;
    QString toTable;
};

// Manages the entire ER diagram
class ERDiagram : public QGraphicsView {
    Q_OBJECT

public:
    explicit ERDiagram(sqlite3* db, QWidget* parent = nullptr);
    ~ERDiagram();

    void generateDiagram();                  // Generate the diagram
    void updateRelationshipPositions();     // Update positions of all relationships

private:
    QGraphicsScene* scene;  // Scene for the diagram
    sqlite3* db;            // SQLite database connection

    struct TableNode {
        TableNodeGroup* group;
        QGraphicsRectItem* rect;
        QGraphicsTextItem* header;
        QVector<QGraphicsTextItem*> fields;
    };

    QMap<QString, TableNode> tableNodes;         // Map of table names to nodes
    QVector<RelationshipLine> relationships;    // List of relationships (lines)

    void createTableNode(const QString& tableName, const QVector<QString>& fields, qreal x, qreal y);
    void createRelationship(const QString& fromTable, const QString& toTable);
};

#endif // ERDIAGRAM_H
