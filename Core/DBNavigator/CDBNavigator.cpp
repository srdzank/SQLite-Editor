#include "CDBNavigator.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <QToolTip>

// Ensure these are paths to your actual icons
const QString tableIconPath = ":/table.png";
const QString columnIconPath = ":/column.png";
const QString indexIconPath = ":/index.png";

CDBNavigator::CDBNavigator(QWidget* parent)
    : QWidget(parent), treeWidget(new QTreeWidget(this))
{
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderLabel("Database Schema");

    // Create and set the layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(treeWidget);
    setLayout(layout);

    // Connect the itemClicked signal to the handleItemClick slot
    connect(treeWidget, &QTreeWidget::itemClicked, this, &CDBNavigator::handleItemClick);

    // Enable mouse tracking to show tooltips
    treeWidget->setMouseTracking(true);
}

CDBNavigator::~CDBNavigator()
{
    if (db) {
        sqlite3_close(db);
    }
}

void CDBNavigator::openDatabase(sqlite3* m_db)
{
    db = m_db;
    populateTreeWidget();
}

std::vector<std::string> CDBNavigator::getTables(sqlite3* db)
{
    std::vector<std::string> tables;
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        return tables;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        tables.emplace_back(reinterpret_cast<const char*>(text));
    }

    sqlite3_finalize(stmt);
    return tables;
}

std::vector<std::pair<std::string, std::string>> CDBNavigator::getTableColumns(const std::string& tableName)
{
    std::vector<std::pair<std::string, std::string>> columns;
    std::string sql = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        qWarning() << "Failed to fetch columns for table" << QString::fromStdString(tableName) << ":" << sqlite3_errmsg(db);
        return columns;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* type = sqlite3_column_text(stmt, 2);
        columns.emplace_back(std::make_pair(reinterpret_cast<const char*>(name), reinterpret_cast<const char*>(type)));
    }

    sqlite3_finalize(stmt);
    return columns;
}

std::vector<std::string> CDBNavigator::getTableIndices(const std::string& tableName)
{
    std::vector<std::string> indices;
    std::string sql = "PRAGMA index_list(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        qWarning() << "Failed to fetch indices for table" << QString::fromStdString(tableName) << ":" << sqlite3_errmsg(db);
        return indices;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* indexName = sqlite3_column_text(stmt, 1);
        indices.emplace_back(reinterpret_cast<const char*>(indexName));
    }

    sqlite3_finalize(stmt);
    return indices;
}

void CDBNavigator::populateTreeWidget()
{
    treeWidget->clear();

    // Load icons
    QIcon tableIcon(tableIconPath);
    QIcon columnIcon(columnIconPath);
    QIcon indexIcon(indexIconPath);

    std::vector<std::string> tables = getTables(db);
    for (const std::string& tableName : tables) {
        QTreeWidgetItem* tableItem = new QTreeWidgetItem(treeWidget);
        tableItem->setText(0, QString::fromStdString(tableName));
        tableItem->setIcon(0, tableIcon); // Set table icon
        tableItem->setToolTip(0, "Table: " + QString::fromStdString(tableName)); // Set tooltip for table

        std::vector<std::pair<std::string, std::string>> columns = getTableColumns(tableName);
        for (const auto& column : columns) {
            QTreeWidgetItem* columnItem = new QTreeWidgetItem(tableItem);
            columnItem->setText(0, QString::fromStdString(column.first) + " (" + QString::fromStdString(column.second) + ")");
            columnItem->setIcon(0, columnIcon); // Set column icon
            columnItem->setToolTip(0, "Column: " + QString::fromStdString(column.first) + "\nType: " + QString::fromStdString(column.second)); // Set tooltip for column
        }

        std::vector<std::string> indices = getTableIndices(tableName);
        for (const auto& index : indices) {
            QTreeWidgetItem* indexItem = new QTreeWidgetItem(tableItem);
            indexItem->setText(0, QString::fromStdString(index));
            indexItem->setIcon(0, indexIcon); // Set index icon
            indexItem->setToolTip(0, "Index: " + QString::fromStdString(index)); // Set tooltip for index
        }
    }
}

void CDBNavigator::handleItemClick(QTreeWidgetItem* item, int column)
{
    if (item->parent() == nullptr) {
        // This is a table item
        QString tableName = item->text(column);
        emit sigClickedTableItem(tableName);
    }
    else {
        // This is a column item or index item
        QString columnName = item->text(column);
        QString tableName = item->parent()->text(0);
 //       QMessageBox::information(this, tr("Item Clicked"), tr("You clicked on: %1\nIn table: %2").arg(columnName, tableName));
    }
}

std::vector<std::string> CDBNavigator::columnNames(const std::string tableName) {
    std::vector<std::string> columns;
    std::string sql = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        qWarning() << "Failed to fetch columns for table" << QString::fromStdString(tableName) << ":" << sqlite3_errmsg(db);
        return columns;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        columns.emplace_back(reinterpret_cast<const char*>(name));
    }

    sqlite3_finalize(stmt);
    return columns;
}
