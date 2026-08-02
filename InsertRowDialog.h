#pragma once

#include <QDialog>
#include <QPair>
#include <QString>
#include <QVector>

class QComboBox;
class QFormLayout;
class QLineEdit;
class QWidget;
class Schema;

// Modal dialog for inserting one new row into an existing table: pick
// the table, then fill in a value per column. A blank field means NULL.
// Like the other schema dialogs, this only collects input - MainWindow
// turns the result into an INSERT INTO statement.
class InsertRowDialog : public QDialog
{
    Q_OBJECT

public:
    InsertRowDialog(const Schema& schema, const QString& initialTable, QWidget* parent = nullptr);

    QString tableName() const;

    // One entry per column of the selected table, in column order:
    // {columnName, rawValueTypedByUser}. An empty value means NULL.
    QVector<QPair<QString, QString>> values() const;

private slots:
    void onTableChanged(const QString& table);

private:
    void accept() override;
    void rebuildFields(const QString& table);

    const Schema& m_schema;
    QComboBox* m_tableCombo = nullptr;
    QWidget* m_fieldsContainer = nullptr;
    QFormLayout* m_fieldsLayout = nullptr;
    QVector<QPair<QString, QLineEdit*>> m_fields;  // column name -> its input widget
};
