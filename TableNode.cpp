#include "TableNode.h"

#include <QBrush>
#include <QCursor>
#include <QFont>
#include <QGraphicsSimpleTextItem>
#include <QPen>

namespace {
constexpr int kTableNameRole = 0;
}

TableNode::TableNode(const QString& tableName, bool isFocus, QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, kWidth, kHeight, parent)
{
    setData(kTableNameRole, tableName);
    setCursor(Qt::PointingHandCursor);

    if (isFocus) {
        setBrush(QBrush(QColor("#0F6E56")));   // teal 600
        setPen(QPen(QColor("#1D9E75"), 1.5));  // teal 400
    } else {
        setBrush(QBrush(QColor("#2C2C2A")));   // gray 900-ish surface
        setPen(QPen(QColor("#5F5E5A"), 1.0));  // gray 600
    }

    auto* label = new QGraphicsSimpleTextItem(tableName, this);
    QFont font = label->font();
    font.setPointSize(9);
    font.setBold(isFocus);
    label->setFont(font);
    label->setBrush(QBrush(QColor(isFocus ? "#E1F5EE" : "#D3D1C7")));

    const QRectF textBounds = label->boundingRect();
    label->setPos((kWidth - textBounds.width()) / 2.0,
                  (kHeight - textBounds.height()) / 2.0);
}

QString TableNode::tableName() const
{
    return data(kTableNameRole).toString();
}