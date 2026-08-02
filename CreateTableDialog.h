#pragma once

#include <QDialog>
#include <QString>
#include <QVector>

class QLineEdit;
class QTableWidget;

// Modal dialog for defining a brand-new table: a name plus a list of
// columns (name / type / PRIMARY KEY / NOT NULL). MainWindow turns the
// result into a CREATE TABLE statement - this dialog only collects and
// validates the definition, it never touches the database itself.
class CreateTableDialog : public QDialog
{
    Q_OBJECT

public:
    struct ColumnDef
    {
        QString name;
        QString type;
        bool notNull = false;
        bool primaryKey = false;
    };

    explicit CreateTableDialog(QWidget* parent = nullptr);

    QString tableName() const;
    QVector<ColumnDef> columns() const;

private slots:
    void addColumnRow();
    void removeSelectedRow();

private:
    void accept() override;

    QLineEdit* m_nameEdit = nullptr;
    QTableWidget* m_columnsTable = nullptr;
};
