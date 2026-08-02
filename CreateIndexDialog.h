#pragma once

#include <QDialog>
#include <QString>
#include <QStringList>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QListWidget;
class Schema;

// Modal dialog for defining a new index: pick a table, an index name,
// which of that table's columns to include, and whether it's UNIQUE.
// Like CreateTableDialog, this only collects and validates input -
// MainWindow turns the result into a CREATE INDEX statement.
class CreateIndexDialog : public QDialog
{
    Q_OBJECT

public:
    // schema must outlive the dialog - MainWindow only ever constructs
    // this on the stack right before calling exec(), so that's always true.
    CreateIndexDialog(const Schema& schema, const QString& initialTable, QWidget* parent = nullptr);

    QString indexName() const;
    QString tableName() const;
    QStringList selectedColumns() const;
    bool isUnique() const;

private slots:
    void onTableChanged(const QString& table);

private:
    void accept() override;

    const Schema& m_schema;
    QLineEdit* m_nameEdit = nullptr;
    QComboBox* m_tableCombo = nullptr;
    QListWidget* m_columnsList = nullptr;
    QCheckBox* m_uniqueCheck = nullptr;
};
