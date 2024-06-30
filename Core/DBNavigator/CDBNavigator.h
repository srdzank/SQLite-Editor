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

    void openDatabase(sqlite3* m_db);
    std::vector<std::string> columnNames(const std::string tableName);

private:
    QTreeWidget* treeWidget;
    sqlite3* db = nullptr;

    std::vector<std::string> getTables(sqlite3* db);
    std::vector<std::pair<std::string, std::string>> getTableColumns(const std::string& tableName);
    std::vector<std::string> getTableIndices(const std::string& tableName); // New method for indices
    void populateTreeWidget();

signals:
    void sigClickedTableItem(const QString&);

private slots:
    void handleItemClick(QTreeWidgetItem* item, int column);
};

#endif // CDBNAVIGATOR_H
