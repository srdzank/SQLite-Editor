#include "CreateIndexDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

#include "Schema.h"

namespace {
const QRegularExpression kIdentifierPattern("^[A-Za-z_][A-Za-z0-9_]*$");
}

CreateIndexDialog::CreateIndexDialog(const Schema& schema, const QString& initialTable, QWidget* parent)
    : QDialog(parent)
    , m_schema(schema)
{
    setWindowTitle("New Index");
    resize(360, 420);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Table:", this));
    m_tableCombo = new QComboBox(this);
    for (const auto& table : schema.tables()) {
        m_tableCombo->addItem(table.name);
    }
    layout->addWidget(m_tableCombo);

    layout->addWidget(new QLabel("Index name:", this));
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setValidator(new QRegularExpressionValidator(kIdentifierPattern, m_nameEdit));
    layout->addWidget(m_nameEdit);

    layout->addWidget(new QLabel("Columns:", this));
    m_columnsList = new QListWidget(this);
    layout->addWidget(m_columnsList, 1);

    m_uniqueCheck = new QCheckBox("UNIQUE", this);
    layout->addWidget(m_uniqueCheck);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Create");
    layout->addWidget(buttons);

    connect(m_tableCombo, &QComboBox::currentTextChanged, this, &CreateIndexDialog::onTableChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &CreateIndexDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &CreateIndexDialog::reject);

    const int initialIndex = m_tableCombo->findText(initialTable);
    if (initialIndex >= 0) {
        m_tableCombo->setCurrentIndex(initialIndex);
    }
    // Populate explicitly rather than relying on currentTextChanged firing
    // for the initial selection, since Qt doesn't guarantee that signal
    // fires just from the combo box being filled or left at index 0.
    if (m_tableCombo->count() > 0) {
        onTableChanged(m_tableCombo->currentText());
    }
}

void CreateIndexDialog::onTableChanged(const QString& table)
{
    m_columnsList->clear();

    const Table* t = m_schema.findTable(table);
    if (!t) {
        return;
    }

    for (const auto& col : t->columns) {
        auto* item = new QListWidgetItem(col.name, m_columnsList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    m_nameEdit->setText("idx_" + table);
}

QString CreateIndexDialog::indexName() const
{
    return m_nameEdit->text().trimmed();
}

QString CreateIndexDialog::tableName() const
{
    return m_tableCombo->currentText();
}

QStringList CreateIndexDialog::selectedColumns() const
{
    QStringList result;
    for (int i = 0; i < m_columnsList->count(); ++i) {
        const auto* item = m_columnsList->item(i);
        if (item->checkState() == Qt::Checked) {
            result << item->text();
        }
    }
    return result;
}

bool CreateIndexDialog::isUnique() const
{
    return m_uniqueCheck->isChecked();
}

void CreateIndexDialog::accept()
{
    if (tableName().isEmpty()) {
        QMessageBox::warning(this, "New Index", "This database has no tables to index.");
        return;
    }
    if (!kIdentifierPattern.match(indexName()).hasMatch()) {
        QMessageBox::warning(this, "New Index",
                              "Index name must start with a letter or underscore, and contain only "
                              "letters, numbers, and underscores.");
        return;
    }
    if (selectedColumns().isEmpty()) {
        QMessageBox::warning(this, "New Index", "Select at least one column.");
        return;
    }

    QDialog::accept();
}
