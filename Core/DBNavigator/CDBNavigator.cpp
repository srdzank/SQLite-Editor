
#include "CDBNavigator.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QFile>

// Ensure these are paths to your actual icons
const QString tableIconPath = ":/table.png";
const QString columnIconPath = ":/column.png";

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
}

CDBNavigator::~CDBNavigator()
{
    if (db) {
        sqlite3_close(db);
    }
}

void CDBNavigator::openDatabase(const QString& fileName)
{
    QByteArray byteArray = fileName.toUtf8();
    const char* filename = byteArray.constData();

    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        QMessageBox::critical(this, tr("Error"), tr("Can't open database: %1").arg(sqlite3_errmsg(db)));
        return;
    }
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

void CDBNavigator::populateTreeWidget()
{
    treeWidget->clear();

    // Load icons
    QIcon tableIcon(tableIconPath);
    QIcon columnIcon(columnIconPath);

    std::vector<std::string> tables = getTables(db);
    for (const std::string& tableName : tables) {
        QTreeWidgetItem* tableItem = new QTreeWidgetItem(treeWidget);
        tableItem->setText(0, QString::fromStdString(tableName));
        tableItem->setIcon(0, tableIcon); // Set table icon

        std::vector<std::pair<std::string, std::string>> columns = getTableColumns(tableName);
        for (const auto& column : columns) {
            QTreeWidgetItem* columnItem = new QTreeWidgetItem(tableItem);
            columnItem->setText(0, QString::fromStdString(column.first) + " (" + QString::fromStdString(column.second) + ")");
            columnItem->setIcon(0, columnIcon); // Set column icon
        }
    }

    treeWidget->expandAll();
}

void CDBNavigator::handleItemClick(QTreeWidgetItem* item, int column)
{
    if (item->parent() == nullptr) {
        // This is a table item
        QString tableName = item->text(column);
        QMessageBox::information(this, tr("Table Clicked"), tr("You clicked on table: %1").arg(tableName));
    }
    else {
        // This is a column item
        QString columnName = item->text(column);
        QString tableName = item->parent()->text(0);
        QMessageBox::information(this, tr("Column Clicked"), tr("You clicked on column: %1\nIn table: %2").arg(columnName, tableName));
    }
}


