#include "CTableEditorDialog.h"

TableEditorDialog::TableEditorDialog(sqlite3* db, QWidget* parent)
    : QDialog(parent), m_db(db) {
    setWindowTitle("SQLite Table Editor");
    resize(600, 400); // Make the dialog larger

    // Table name input
    QLabel* tableNameLabel = new QLabel("Table Name:");
    tableNameInput = new QLineEdit(this);

    // Column Table
    columnTable = new QTableWidget(this);
    columnTable->setColumnCount(4);
    columnTable->setHorizontalHeaderLabels({ "Column Name", "Data Type", "NOT NULL", "Primary Key" });
    columnTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Buttons
    addColumnButton = new QPushButton("Add Column", this);
    removeColumnButton = new QPushButton("Remove Column", this);
    saveButton = new QPushButton("Save Table", this);

    // Layouts
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addColumnButton);
    buttonLayout->addWidget(removeColumnButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableNameLabel);
    mainLayout->addWidget(tableNameInput);
    mainLayout->addWidget(columnTable);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(saveButton);

    setLayout(mainLayout);

    // Connect Signals
    connect(addColumnButton, &QPushButton::clicked, this, &TableEditorDialog::addColumn);
    connect(removeColumnButton, &QPushButton::clicked, this, &TableEditorDialog::removeColumn);
    connect(saveButton, &QPushButton::clicked, this, &TableEditorDialog::saveTable);
}

void TableEditorDialog::addColumn() {
    int row = columnTable->rowCount();
    columnTable->insertRow(row);

    // Set up a combo box for SQLite data types
    QComboBox* typeCombo = createTypeComboBox();
    columnTable->setCellWidget(row, 1, typeCombo);

    // Optional defaults for NOT NULL and Primary Key
    columnTable->setItem(row, 2, new QTableWidgetItem(""));
    columnTable->setItem(row, 3, new QTableWidgetItem(""));
}

void TableEditorDialog::removeColumn() {
    int currentRow = columnTable->currentRow();
    if (currentRow >= 0) {
        columnTable->removeRow(currentRow);
    }
}

void TableEditorDialog::loadExistingTable(const QString& tableName) {
    m_existingTable = tableName;
    tableNameInput->setText(tableName);
    tableNameInput->setReadOnly(true);
    loadTableSchema(tableName);
}

void TableEditorDialog::loadTableSchema(const QString& tableName) {
    QString queryStr = QString("PRAGMA table_info(%1);").arg(tableName);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, queryStr.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to load table schema.");
        return;
    }

    columnTable->setRowCount(0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int row = columnTable->rowCount();
        columnTable->insertRow(row);

        columnTable->setItem(row, 0, new QTableWidgetItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));

        QComboBox* typeCombo = createTypeComboBox();
        typeCombo->setCurrentText(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        columnTable->setCellWidget(row, 1, typeCombo);

        columnTable->setItem(row, 2, new QTableWidgetItem(sqlite3_column_int(stmt, 3) ? "YES" : ""));
        columnTable->setItem(row, 3, new QTableWidgetItem(sqlite3_column_int(stmt, 5) ? "YES" : ""));
    }

    sqlite3_finalize(stmt);
}

void TableEditorDialog::saveTable() {
    QString createQuery = generateCreateTableQuery();

    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, createQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to save table: %1").arg(errMsg));
        sqlite3_free(errMsg);
        return;
    }

    QMessageBox::information(this, "Success", "Table created/updated successfully.");
    accept();
}

QString TableEditorDialog::generateCreateTableQuery() {
    QString queryStr = "CREATE TABLE ";
    queryStr += tableNameInput->text() + " (";

    QStringList columnDefinitions;
    for (int i = 0; i < columnTable->rowCount(); ++i) {
        QString columnName = columnTable->item(i, 0)->text();
        QComboBox* typeCombo = qobject_cast<QComboBox*>(columnTable->cellWidget(i, 1));
        QString dataType = typeCombo ? typeCombo->currentText() : "";

        QString notNull = columnTable->item(i, 2) && columnTable->item(i, 2)->text() == "YES" ? "NOT NULL" : "";
        QString primaryKey = columnTable->item(i, 3) && columnTable->item(i, 3)->text() == "YES" ? "PRIMARY KEY" : "";

        QString columnDef = columnName + " " + dataType;
        if (!notNull.isEmpty()) columnDef += " " + notNull;
        if (!primaryKey.isEmpty()) columnDef += " " + primaryKey;

        columnDefinitions.append(columnDef);
    }

    queryStr += columnDefinitions.join(", ") + ");";
    return queryStr;
}

QComboBox* TableEditorDialog::createTypeComboBox() {
    QComboBox* combo = new QComboBox(this);
    combo->setEditable(true); // Allow custom data types
    combo->addItems({ "INTEGER", "TEXT", "REAL", "BLOB", "NUMERIC" }); // Predefined SQLite types
    return combo;
}
