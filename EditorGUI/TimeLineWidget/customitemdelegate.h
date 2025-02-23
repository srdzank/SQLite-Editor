#ifndef CUSTOMITEMDELEGATE_H
#define CUSTOMITEMDELEGATE_H

#include <QStyledItemDelegate>

class CustomItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CustomItemDelegate(QObject *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CUSTOMITEMDELEGATE_H
