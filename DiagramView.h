#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QString>
#include <QVector>

#include "Schema.h"

// Renders a focus-based diagram: the focused table sits in the center,
// its direct relations (from Schema::relationsFor) are placed around it
// in a cross layout. Clicking any visible table emits tableClicked, which
// MainWindow uses to re-focus the diagram - this is the "spanning tree
// around the active table" behaviour discussed for phase 2.
class DiagramView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit DiagramView(QWidget* parent = nullptr);

    // Redraws the diagram centered on focusTable, using relations from schema.
    void setFocusTable(const Schema& schema, const QString& focusTable);

    // Empties the diagram - used when no table is focused (e.g. right
    // after a new/empty database is opened, or the focused table itself
    // was just dropped).
    void clear();

    // Redraws the diagram centered on focusTable, but with the related
    // tables supplied explicitly rather than looked up via Schema::
    // relationsFor. Used to drive the diagram from raw, manually-typed
    // SQL (FROM/JOIN table names) so typing keeps the visual in sync.
    // A solid accent line is drawn when a real foreign key connects the
    // pair; a dashed line otherwise (e.g. a JOIN typed without a known
    // relation).
    void setTablesFromNames(const Schema& schema, const QString& focusTable,
                             const QVector<QString>& relatedTables);

signals:
    void tableClicked(const QString& tableName);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QGraphicsScene m_scene;
};
