#pragma once

#include <QDialog>
#include <QStringList>

class QCheckBox;
class QListWidget;
class Schema;

// Modal dialog for choosing what to export: which tables, and whether to
// include structure (CREATE TABLE / CREATE INDEX) and/or data (INSERT
// statements). MainWindow does the actual work of writing the .sql file -
// this only collects the selection.
class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(const Schema& schema, QWidget* parent = nullptr);

    QStringList selectedTables() const;
    bool includeStructure() const;
    bool includeData() const;

private slots:
    void selectAll();
    void selectNone();

private:
    void accept() override;

    QListWidget* m_tableList = nullptr;
    QCheckBox* m_structureCheck = nullptr;
    QCheckBox* m_dataCheck = nullptr;
};
