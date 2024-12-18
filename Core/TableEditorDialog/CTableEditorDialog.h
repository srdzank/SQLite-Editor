#ifndef CTABLEMANAGERDIALOG_H
#define CTABLEMANAGERDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <sqlite3.h>

class CTableManagerDialog : public QDialog {
    Q_OBJECT

public:
    explicit CTableManagerDialog(sqlite3* db, QWidget* parent = nullptr);

private slots:
    void createTable();
    void deleteTable();
    void loadTableSchema(const QString& tableName);
    void saveTableChanges();
    void addColumn();
    void removeColumn();
    void refreshTableList();

private:
    sqlite3* m_db;               // SQLite database connection
    QString m_currentTable;      // Name of the current table being edited

    // UI Elements
    QComboBox* tableSelector;    // Dropdown to select a table
    QLineEdit* tableNameInput;   // Input for new table name
    QTableWidget* columnTable;   // Table for managing columns
    QPushButton* addColumnButton;
    QPushButton* removeColumnButton;
    QPushButton* createTableButton;
    QPushButton* deleteTableButton;
    QPushButton* saveChangesButton;

    QString generateCreateTableQuery(const QString& tableNameOverride = QString());
    bool alterTable();
    QComboBox* createTypeComboBox();
};

#endif // CTABLEMANAGERDIALOG_H
