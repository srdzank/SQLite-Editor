#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>


class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = nullptr);
    ~CustomWidget();

private slots:
    void addItem();
    void deleteItem();
    void onItemClicked(QListWidgetItem *item);

private:
    QVBoxLayout *layout;
    QList<QWidget*> widgetList;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QListWidget *listWidget;
    QWidget *createNewItem(const QString &text);
};

#endif // CUSTOMWIDGET_H
