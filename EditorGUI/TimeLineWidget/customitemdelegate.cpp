#include "customitemdelegate.h"

CustomItemDelegate::CustomItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize CustomItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(30); // Set item height to 50
    return size;
}
