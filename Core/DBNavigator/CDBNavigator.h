#ifndef CDBNAVIGATOR_H
#define CDBNAVIGATOR_H

#include <QWidget>
#include <QTreeWidget>
#include <sqlite3.h>
#include <vector>
#include <string>

class CDBNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit CDBNavigator(QWidget* parent = nullptr);
    ~CDBNavigator();

    void openDatabase(const QString& fileName);

private:
    QTreeWidget* treeWidget;
    sqlite3* db = nullptr;

    std::vector<std::string> getTables(sqlite3* db);
    std::vector<std::pair<std::string, std::string>> getTableColumns(const std::string& tableName);
    void populateTreeWidget();

private slots:
    void handleItemClick(QTreeWidgetItem* item, int column);
};

#endif // CDBNAVIGATOR_H
