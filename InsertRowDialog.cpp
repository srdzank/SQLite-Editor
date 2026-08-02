#include "InsertRowDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "Schema.h"

InsertRowDialog::InsertRowDialog(const Schema& schema, const QString& initialTable, QWidget* parent)
    : QDialog(parent)
    , m_schema(schema)
{
    setWindowTitle("Insert Row");
    resize(380, 420);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Table:", this));
    m_tableCombo = new QComboBox(this);
    for (const auto& table : schema.tables()) {
        m_tableCombo->addItem(table.name);
    }
    layout->addWidget(m_tableCombo);

    // Columns are shown in a scroll area since a wide table could
    // otherwise make the dialog unreasonably tall.
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    m_fieldsContainer = new QWidget();
    m_fieldsLayout = new QFormLayout(m_fieldsContainer);
    scrollArea->setWidget(m_fieldsContainer);
    layout->addWidget(scrollArea, 1);

    auto* hint = new QLabel("Leave a field blank to insert NULL.", this);
    hint->setStyleSheet("color: #888780; font-size: 10px;");
    layout->addWidget(hint);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Insert");
    layout->addWidget(buttons);

    connect(m_tableCombo, &QComboBox::currentTextChanged, this, &InsertRowDialog::onTableChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &InsertRowDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &InsertRowDialog::reject);

    const int initialIndex = m_tableCombo->findText(initialTable);
    if (initialIndex >= 0) {
        m_tableCombo->setCurrentIndex(initialIndex);
    }
    // Populate explicitly rather than relying on currentTextChanged firing
    // for the initial selection - Qt doesn't guarantee that.
    if (m_tableCombo->count() > 0) {
        onTableChanged(m_tableCombo->currentText());
    }
}

void InsertRowDialog::onTableChanged(const QString& table)
{
    rebuildFields(table);
}

void InsertRowDialog::rebuildFields(const QString& table)
{
    while (m_fieldsLayout->rowCount() > 0) {
        m_fieldsLayout->removeRow(0);
    }
    m_fields.clear();

    const Table* t = m_schema.findTable(table);
    if (!t) {
        return;
    }

    for (const auto& col : t->columns) {
        auto* edit = new QLineEdit(m_fieldsContainer);
        QString labelText = col.name + "  (" + col.type + ")";
        if (col.isPrimaryKey) {
            labelText += "  [PK]";
        }
        m_fieldsLayout->addRow(labelText, edit);
        m_fields.push_back({col.name, edit});
    }
}

QString InsertRowDialog::tableName() const
{
    return m_tableCombo->currentText();
}

QVector<QPair<QString, QString>> InsertRowDialog::values() const
{
    QVector<QPair<QString, QString>> result;
    for (const auto& field : m_fields) {
        result.push_back({field.first, field.second->text()});
    }
    return result;
}

void InsertRowDialog::accept()
{
    if (tableName().isEmpty()) {
        QMessageBox::warning(this, "Insert Row", "This database has no tables.");
        return;
    }
    QDialog::accept();
}
