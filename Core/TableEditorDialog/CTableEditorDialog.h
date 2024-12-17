#ifndef TABLEEDITORDIALOG_H
#define TABLEEDITORDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QHeaderView>
#include <QMessageBox>
#include <sqlite3.h>

class TableEditorDialog : public QDialog {
    Q_OBJECT

public:
    explicit TableEditorDialog(sqlite3* db, QWidget* parent = nullptr);
    void loadExistingTable(const QString& tableName);

private slots:
    void addColumn();
    void removeColumn();
    void saveTable();

private:
    sqlite3* m_db;
    QString m_existingTable;

    QLineEdit* tableNameInput;
    QTableWidget* columnTable;
    QPushButton* addColumnButton;
    QPushButton* removeColumnButton;
    QPushButton* saveButton;

    QString generateCreateTableQuery();
    void loadTableSchema(const QString& tableName);
    QComboBox* createTypeComboBox(); // Helper to create type dropdown
};

#endif // TABLEEDITORDIALOG_H
