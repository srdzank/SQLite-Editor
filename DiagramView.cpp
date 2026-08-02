#include "DiagramView.h"

#include <QMouseEvent>
#include <QPen>
#include <QPointF>
#include <QSet>

#include "TableNode.h"

DiagramView::DiagramView(QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(&m_scene);
    setRenderHint(QPainter::Antialiasing);
    setStyleSheet("background-color: #17171A; border: none;");
    m_scene.setBackgroundBrush(QColor("#17171A"));
}

void DiagramView::setFocusTable(const Schema& schema, const QString& focusTable)
{
    m_scene.clear();

    const Table* table = schema.findTable(focusTable);
    if (!table) {
        return;
    }

    // Collect unique neighbour table names from the focus table's
    // relations - this is the "spanning tree" step: only direct
    // relations are shown, not the whole schema.
    QVector<QString> neighbours;
    QSet<QString> seen;
    for (const auto& fk : schema.relationsFor(focusTable)) {
        const QString other = (fk.fromTable == focusTable) ? fk.toTable : fk.fromTable;
        if (other != focusTable && !seen.contains(other)) {
            seen.insert(other);
            neighbours.push_back(other);
        }
    }

    const QPointF center(160, 120);
    // Cross layout: up to 4 neighbours placed top / left / right / bottom.
    const QVector<QPointF> neighbourSlots = {
        QPointF(160, 30),   // top
        QPointF(50, 120),   // left
        QPointF(270, 120),  // right
        QPointF(160, 210),  // bottom
    };

    auto placeNode = [this](const QString& name, const QPointF& centerPos, bool isFocus) {
        auto* node = new TableNode(name, isFocus);
        node->setPos(centerPos.x() - TableNode::kWidth / 2.0,
                     centerPos.y() - TableNode::kHeight / 2.0);
        m_scene.addItem(node);
        return centerPos;
    };

    // Draw connector lines first so nodes render on top of them.
    QPen linePen(QColor("#5F5E5A"), 1.2);
    for (int i = 0; i < neighbours.size() && i < neighbourSlots.size(); ++i) {
        m_scene.addLine(center.x(), center.y(), neighbourSlots[i].x(), neighbourSlots[i].y(), linePen);
    }

    placeNode(focusTable, center, true);
    for (int i = 0; i < neighbours.size() && i < neighbourSlots.size(); ++i) {
        placeNode(neighbours[i], neighbourSlots[i], false);
    }

    m_scene.setSceneRect(0, 0, 330, 240);
}

void DiagramView::clear()
{
    m_scene.clear();
    m_scene.setSceneRect(0, 0, 330, 240);
}

void DiagramView::setTablesFromNames(const Schema& schema, const QString& focusTable,
                                      const QVector<QString>& relatedTables)
{
    m_scene.clear();

    if (!schema.findTable(focusTable)) {
        return;
    }

    // Keep only real, distinct tables other than the focus itself.
    QVector<QString> neighbours;
    QSet<QString> seen;
    for (const auto& name : relatedTables) {
        if (name == focusTable || seen.contains(name) || !schema.findTable(name)) {
            continue;
        }
        seen.insert(name);
        neighbours.push_back(name);
    }

    // True if a real foreign key connects focusTable and other.
    auto hasKnownRelation = [&schema](const QString& lhs, const QString& rhs) {
        for (const auto& fk : schema.relationsFor(lhs)) {
            if ((fk.fromTable == lhs && fk.toTable == rhs)
                || (fk.toTable == lhs && fk.fromTable == rhs)) {
                return true;
            }
        }
        return false;
    };

    const QPointF center(160, 120);
    // Cross layout: up to 4 neighbours placed top / left / right / bottom.
    const QVector<QPointF> neighbourSlots = {
        QPointF(160, 30),   // top
        QPointF(50, 120),   // left
        QPointF(270, 120),  // right
        QPointF(160, 210),  // bottom
    };

    auto placeNode = [this](const QString& name, const QPointF& centerPos, bool isFocus) {
        auto* node = new TableNode(name, isFocus);
        node->setPos(centerPos.x() - TableNode::kWidth / 2.0,
                     centerPos.y() - TableNode::kHeight / 2.0);
        m_scene.addItem(node);
    };

    // Solid accent line for a real foreign key (as typed in JOIN ... ON),
    // dashed gray line for a table just named in the SQL without one -
    // e.g. a cross join or a relation the schema doesn't declare.
    QPen relatedPen(QColor("#1D9E75"), 1.4);
    QPen unrelatedPen(QColor("#5F5E5A"), 1.0, Qt::DashLine);

    for (int i = 0; i < neighbours.size() && i < neighbourSlots.size(); ++i) {
        const QPen& pen = hasKnownRelation(focusTable, neighbours[i]) ? relatedPen : unrelatedPen;
        m_scene.addLine(center.x(), center.y(), neighbourSlots[i].x(), neighbourSlots[i].y(), pen);
    }

    placeNode(focusTable, center, true);
    for (int i = 0; i < neighbours.size() && i < neighbourSlots.size(); ++i) {
        placeNode(neighbours[i], neighbourSlots[i], false);
    }

    m_scene.setSceneRect(0, 0, 330, 240);
}

void DiagramView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsItem* item = itemAt(event->pos());
    if (item) {
        item = item->topLevelItem();
        if (auto* node = dynamic_cast<TableNode*>(item)) {
            emit tableClicked(node->tableName());
        }
    }
    QGraphicsView::mousePressEvent(event);
}