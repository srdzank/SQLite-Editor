#include "CreateTableDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSet>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

// Only plain identifiers are accepted for table/column names - keeps the
// generated SQL simple and avoids needing to quote or escape anything.
const QRegularExpression kIdentifierPattern("^[A-Za-z_][A-Za-z0-9_]*$");

const QStringList kColumnTypes = {"INTEGER", "TEXT", "REAL", "NUMERIC", "BLOB"};

enum ColumnIndex { NameColumn, TypeColumn, PrimaryKeyColumn, NotNullColumn, ColumnCount };

// PK/NOT NULL cells hold a centered checkbox rather than a plain
// checkable item, so it lines up visually with the combo box column.
QWidget* centeredCheckBox()
{
    auto* container = new QWidget();
    auto* checkBox = new QCheckBox(container);
    auto* checkLayout = new QHBoxLayout(container);
    checkLayout->addWidget(checkBox);
    checkLayout->setAlignment(Qt::AlignCenter);
    checkLayout->setContentsMargins(0, 0, 0, 0);
    return container;
}

} // namespace

CreateTableDialog::CreateTableDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("New Table");
    resize(480, 360);

    auto* layout = new QVBoxLayout(this);

    auto* nameRow = new QHBoxLayout();
    nameRow->addWidget(new QLabel("Table name:", this));
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setValidator(new QRegularExpressionValidator(kIdentifierPattern, m_nameEdit));
    nameRow->addWidget(m_nameEdit, 1);
    layout->addLayout(nameRow);

    m_columnsTable = new QTableWidget(0, ColumnCount, this);
    m_columnsTable->setHorizontalHeaderLabels({"Column", "Type", "PK", "NOT NULL"});
    m_columnsTable->horizontalHeader()->setSectionResizeMode(NameColumn, QHeaderView::Stretch);
    m_columnsTable->horizontalHeader()->setSectionResizeMode(TypeColumn, QHeaderView::ResizeToContents);
    m_columnsTable->horizontalHeader()->setSectionResizeMode(PrimaryKeyColumn, QHeaderView::ResizeToContents);
    m_columnsTable->horizontalHeader()->setSectionResizeMode(NotNullColumn, QHeaderView::ResizeToContents);
    m_columnsTable->verticalHeader()->setVisible(false);
    layout->addWidget(m_columnsTable, 1);

    auto* rowButtons = new QHBoxLayout();
    auto* addRowBtn = new QPushButton("+ Add column", this);
    auto* removeRowBtn = new QPushButton("- Remove column", this);
    rowButtons->addWidget(addRowBtn);
    rowButtons->addWidget(removeRowBtn);
    rowButtons->addStretch();
    layout->addLayout(rowButtons);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Create");
    layout->addWidget(buttons);

    connect(addRowBtn, &QPushButton::clicked, this, &CreateTableDialog::addColumnRow);
    connect(removeRowBtn, &QPushButton::clicked, this, &CreateTableDialog::removeSelectedRow);
    connect(buttons, &QDialogButtonBox::accepted, this, &CreateTableDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &CreateTableDialog::reject);

    addColumnRow();  // start with one empty row so the dialog isn't blank
}

void CreateTableDialog::addColumnRow()
{
    const int row = m_columnsTable->rowCount();
    m_columnsTable->insertRow(row);

    m_columnsTable->setItem(row, NameColumn, new QTableWidgetItem());

    auto* typeCombo = new QComboBox();
    typeCombo->addItems(kColumnTypes);
    m_columnsTable->setCellWidget(row, TypeColumn, typeCombo);

    m_columnsTable->setCellWidget(row, PrimaryKeyColumn, centeredCheckBox());
    m_columnsTable->setCellWidget(row, NotNullColumn, centeredCheckBox());
}

void CreateTableDialog::removeSelectedRow()
{
    const int row = m_columnsTable->currentRow();
    if (row >= 0) {
        m_columnsTable->removeRow(row);
    }
}

QString CreateTableDialog::tableName() const
{
    return m_nameEdit->text().trimmed();
}

QVector<CreateTableDialog::ColumnDef> CreateTableDialog::columns() const
{
    QVector<ColumnDef> result;
    for (int row = 0; row < m_columnsTable->rowCount(); ++row) {
        ColumnDef col;
        col.name = m_columnsTable->item(row, NameColumn)->text().trimmed();
        col.type = static_cast<QComboBox*>(m_columnsTable->cellWidget(row, TypeColumn))->currentText();
        col.primaryKey = m_columnsTable->cellWidget(row, PrimaryKeyColumn)->findChild<QCheckBox*>()->isChecked();
        col.notNull = m_columnsTable->cellWidget(row, NotNullColumn)->findChild<QCheckBox*>()->isChecked();
        result.push_back(col);
    }
    return result;
}

void CreateTableDialog::accept()
{
    if (!kIdentifierPattern.match(tableName()).hasMatch()) {
        QMessageBox::warning(this, "New Table",
                              "Table name must start with a letter or underscore, and contain only "
                              "letters, numbers, and underscores.");
        return;
    }

    const auto cols = columns();
    if (cols.isEmpty()) {
        QMessageBox::warning(this, "New Table", "Add at least one column.");
        return;
    }

    QSet<QString> seenNames;
    for (const auto& col : cols) {
        if (!kIdentifierPattern.match(col.name).hasMatch()) {
            QMessageBox::warning(this, "New Table",
                                  "Every column name must start with a letter or underscore, and "
                                  "contain only letters, numbers, and underscores.");
            return;
        }
        const QString key = col.name.toLower();
        if (seenNames.contains(key)) {
            QMessageBox::warning(this, "New Table",
                                  "Column names must be unique: \"" + col.name + "\" is repeated.");
            return;
        }
        seenNames.insert(key);
    }

    QDialog::accept();
}
