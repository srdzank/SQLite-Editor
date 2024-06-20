#include "customwidget.h"
#include <QLabel>
#include "../logger.h"
#include "customitemdelegate.h"


CustomWidget::CustomWidget(QWidget *parent)
    : QWidget(parent)
{
    layout = new QVBoxLayout(this);

    addButton = new QPushButton("Add Item", this);
    deleteButton = new QPushButton("Delete Item", this);
    listWidget = new QListWidget(this);
    listWidget->setItemDelegate(new CustomItemDelegate(this));

    layout->addWidget(addButton);
    layout->addWidget(deleteButton);
    layout->addWidget(listWidget);

    connect(addButton, &QPushButton::clicked, this, &CustomWidget::addItem);
    connect(deleteButton, &QPushButton::clicked, this, &CustomWidget::deleteItem);
    connect(listWidget, &QListWidget::itemClicked, this, &CustomWidget::onItemClicked);

    setLayout(layout);
    LOG("TimeLineWidget is created");
}

CustomWidget::~CustomWidget()
{
    qDeleteAll(widgetList);
}

void CustomWidget::addItem()
{
    QString itemName = QString("Item %1").arg(widgetList.size() + 1);
    QWidget *newItem = createNewItem(itemName);
    widgetList.append(newItem);

    QListWidgetItem *listItem = new QListWidgetItem(itemName, listWidget);
    listItem->setSizeHint(QSize(listItem->sizeHint().width(), 30)); // Set item height to 50
    // Set the custom widget in the list item
    QWidget *itemWidget = new QWidget();
    if ((widgetList.size() + 1)%2 == 0) {
        itemWidget->setStyleSheet("QWidget { background-color: lightgray; }");
    }else{
        itemWidget->setStyleSheet("QWidget { background-color: white; }");
    }
    listWidget->setItemWidget(listItem, itemWidget);

    LOG("Item added. Total items:" + QString::number(widgetList.size()));
}

void CustomWidget::deleteItem()
{
    if (!listWidget->selectedItems().isEmpty()) {
        QListWidgetItem *item = listWidget->takeItem(listWidget->row(listWidget->selectedItems().first()));
        QString itemName = item->text();

        // Find and remove the corresponding QWidget from widgetList
        for (int i = 0; i < widgetList.size(); ++i) {
            if (static_cast<QLabel*>(widgetList.at(i))->text() == itemName) {
                QWidget *widgetToRemove = widgetList.takeAt(i);
                delete widgetToRemove;
                break;
            }
        }

        delete item;

        LOG("Item deleted. Total items:" + QString::number(widgetList.size()));

    }
}

QWidget* CustomWidget::createNewItem(const QString &text)
{
    QLabel *label = new QLabel(text, this);
    label->setFixedHeight(30);
    return label;
}

void CustomWidget::onItemClicked(QListWidgetItem *item)
{
    LOG("Item clicked:" + item->text());

}
