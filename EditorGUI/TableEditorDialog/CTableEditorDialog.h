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
#include <QInputDialog> // Include QInputDialog
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
    void manageIndexes(); // Slot to open the index management dialog
    void createIndex();
    void deleteIndex();
    void addForeignKey();
    void removeForeignKey();

private:
    void populateIndexes(QTableWidget* indexesTable); // Helper to fetch and display indexes
    void populateForeignKeys();                      // Populate foreign key relationships

    sqlite3* m_db;               // SQLite database connection
    QString m_currentTable;      // Name of the current table being edited

    // UI Elements
    QComboBox* tableSelector;    // Dropdown to select a table
    QLineEdit* tableNameInput;   // Input for new table name
    QTableWidget* columnTable;   // Table for managing columns
    QTableWidget* foreignKeyTable; // Table for managing foreign keys
    QPushButton* addColumnButton;
    QPushButton* removeColumnButton;
    QPushButton* createTableButton;
    QPushButton* deleteTableButton;
    QPushButton* saveChangesButton;
    QPushButton* manageIndexesButton; // Button for index management
    QPushButton* addForeignKeyButton;
    QPushButton* removeForeignKeyButton;

    QString generateCreateTableQuery(const QString& tableNameOverride = QString());
    bool alterTable();
    QComboBox* createTypeComboBox();
};

#endif // CTABLEMANAGERDIALOG_H
