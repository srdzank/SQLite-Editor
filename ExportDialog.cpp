#include "ExportDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "Schema.h"

ExportDialog::ExportDialog(const Schema& schema, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Export Database");
    resize(360, 440);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Tables to export:", this));
    m_tableList = new QListWidget(this);
    for (const auto& table : schema.tables()) {
        auto* item = new QListWidgetItem(table.name, m_tableList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);  // exporting everything is the common case
    }
    layout->addWidget(m_tableList, 1);

    auto* selectButtons = new QHBoxLayout();
    auto* selectAllBtn = new QPushButton("Select All", this);
    auto* selectNoneBtn = new QPushButton("Select None", this);
    selectButtons->addWidget(selectAllBtn);
    selectButtons->addWidget(selectNoneBtn);
    selectButtons->addStretch();
    layout->addLayout(selectButtons);

    m_structureCheck = new QCheckBox("Structure (CREATE TABLE / indexes)", this);
    m_structureCheck->setChecked(true);
    layout->addWidget(m_structureCheck);

    m_dataCheck = new QCheckBox("Data (INSERT rows)", this);
    m_dataCheck->setChecked(true);
    layout->addWidget(m_dataCheck);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Export");
    layout->addWidget(buttons);

    connect(selectAllBtn, &QPushButton::clicked, this, &ExportDialog::selectAll);
    connect(selectNoneBtn, &QPushButton::clicked, this, &ExportDialog::selectNone);
    connect(buttons, &QDialogButtonBox::accepted, this, &ExportDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ExportDialog::reject);
}

void ExportDialog::selectAll()
{
    for (int i = 0; i < m_tableList->count(); ++i) {
        m_tableList->item(i)->setCheckState(Qt::Checked);
    }
}

void ExportDialog::selectNone()
{
    for (int i = 0; i < m_tableList->count(); ++i) {
        m_tableList->item(i)->setCheckState(Qt::Unchecked);
    }
}

QStringList ExportDialog::selectedTables() const
{
    QStringList result;
    for (int i = 0; i < m_tableList->count(); ++i) {
        const auto* item = m_tableList->item(i);
        if (item->checkState() == Qt::Checked) {
            result << item->text();
        }
    }
    return result;
}

bool ExportDialog::includeStructure() const
{
    return m_structureCheck->isChecked();
}

bool ExportDialog::includeData() const
{
    return m_dataCheck->isChecked();
}

void ExportDialog::accept()
{
    if (selectedTables().isEmpty()) {
        QMessageBox::warning(this, "Export Database", "Select at least one table.");
        return;
    }
    if (!includeStructure() && !includeData()) {
        QMessageBox::warning(this, "Export Database", "Choose at least Structure or Data to export.");
        return;
    }
    QDialog::accept();
}
