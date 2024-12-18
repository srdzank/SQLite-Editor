#include "CTableEditorDialog.h"
#include <QHeaderView>

CTableManagerDialog::CTableManagerDialog(sqlite3* db, QWidget* parent)
    : QDialog(parent), m_db(db) {
    setWindowTitle("SQLite Table Manager");
    resize(700, 500);

    // UI setup
    QLabel* tableSelectorLabel = new QLabel("Select Table:");
    tableSelector = new QComboBox(this);

    QLabel* tableNameLabel = new QLabel("Table Name:");
    tableNameInput = new QLineEdit(this);

    columnTable = new QTableWidget(this);
    columnTable->setColumnCount(4);
    columnTable->setHorizontalHeaderLabels({ "Column Name", "Data Type", "NOT NULL", "Primary Key" });
    columnTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Buttons
    createTableButton = new QPushButton("Create Table", this);
    deleteTableButton = new QPushButton("Delete Table", this);
    addColumnButton = new QPushButton("Add Column", this);
    removeColumnButton = new QPushButton("Remove Column", this);
    saveChangesButton = new QPushButton("Save Changes", this);

    // Layouts
    QHBoxLayout* tableSelectorLayout = new QHBoxLayout();
    tableSelectorLayout->addWidget(tableSelectorLabel);
    tableSelectorLayout->addWidget(tableSelector);

    QHBoxLayout* tableNameLayout = new QHBoxLayout();
    tableNameLayout->addWidget(tableNameLabel);
    tableNameLayout->addWidget(tableNameInput);

    QHBoxLayout* columnButtonLayout = new QHBoxLayout();
    columnButtonLayout->addWidget(addColumnButton);
    columnButtonLayout->addWidget(removeColumnButton);

    QHBoxLayout* actionButtonLayout = new QHBoxLayout();
    actionButtonLayout->addWidget(createTableButton);
    actionButtonLayout->addWidget(deleteTableButton);
    actionButtonLayout->addWidget(saveChangesButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(tableSelectorLayout);
    mainLayout->addLayout(tableNameLayout);
    mainLayout->addWidget(columnTable);
    mainLayout->addLayout(columnButtonLayout);
    mainLayout->addLayout(actionButtonLayout);

    setLayout(mainLayout);

    // Populate table list
    refreshTableList();

    // Connect signals
    connect(createTableButton, &QPushButton::clicked, this, &CTableManagerDialog::createTable);
    connect(deleteTableButton, &QPushButton::clicked, this, &CTableManagerDialog::deleteTable);
    connect(addColumnButton, &QPushButton::clicked, this, &CTableManagerDialog::addColumn);
    connect(removeColumnButton, &QPushButton::clicked, this, &CTableManagerDialog::removeColumn);
    connect(saveChangesButton, &QPushButton::clicked, this, &CTableManagerDialog::saveTableChanges);
    connect(tableSelector, &QComboBox::currentTextChanged, this, &CTableManagerDialog::loadTableSchema);
}

void CTableManagerDialog::refreshTableList() {
    tableSelector->clear();
    sqlite3_stmt* stmt;

    const char* query = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to fetch table list.");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        tableSelector->addItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }

    sqlite3_finalize(stmt);
}

void CTableManagerDialog::createTable() {
    QString createQuery = generateCreateTableQuery();
    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, createQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to create table: %1").arg(errMsg));
        sqlite3_free(errMsg);
        return;
    }

    QMessageBox::information(this, "Success", "Table created successfully.");
    refreshTableList();
}

void CTableManagerDialog::deleteTable() {
    QString tableName = tableSelector->currentText();
    if (tableName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No table selected.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deletion",
        QString("Are you sure you want to delete the table '%1'?").arg(tableName),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        QString deleteQuery = QString("DROP TABLE IF EXISTS %1;").arg(tableName);
        char* errMsg = nullptr;

        if (sqlite3_exec(m_db, deleteQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            QMessageBox::critical(this, "Error", QString("Failed to delete table: %1").arg(errMsg));
            sqlite3_free(errMsg);
            return;
        }

        QMessageBox::information(this, "Success", QString("Table '%1' deleted successfully.").arg(tableName));
        refreshTableList();
    }
}

void CTableManagerDialog::loadTableSchema(const QString& tableName) {
    m_currentTable = tableName;
    columnTable->setRowCount(0);

    QString query = QString("PRAGMA table_info(%1);").arg(tableName);
    sqlite3_stmt* stmt;

    if (tableName.isNull()) {
        return;
    }

    if (sqlite3_prepare_v2(m_db, query.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to load table schema.");
        return;
    }

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

void CTableManagerDialog::saveTableChanges() {
    if (alterTable()) {
        QMessageBox::information(this, "Success", "Table changes saved successfully.");
        refreshTableList();
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to save table changes.");
    }
}

void CTableManagerDialog::addColumn() {
    int row = columnTable->rowCount();
    columnTable->insertRow(row);

    columnTable->setItem(row, 0, new QTableWidgetItem(""));
    columnTable->setCellWidget(row, 1, createTypeComboBox());
    columnTable->setItem(row, 2, new QTableWidgetItem(""));
    columnTable->setItem(row, 3, new QTableWidgetItem(""));
}

void CTableManagerDialog::removeColumn() {
    int currentRow = columnTable->currentRow();
    if (currentRow >= 0) {
        columnTable->removeRow(currentRow);
    }
}

QString CTableManagerDialog::generateCreateTableQuery(const QString& tableNameOverride) {
    QString tableName = tableNameOverride.isEmpty() ? tableNameInput->text() : tableNameOverride;
    QString queryStr = "CREATE TABLE " + tableName + " (";

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

bool CTableManagerDialog::alterTable() {
    QString tempTableName = m_currentTable + "_temp";

    QString createQuery = generateCreateTableQuery(tempTableName);
    QString copyDataQuery = QString("INSERT INTO %1 SELECT * FROM %2;").arg(tempTableName, m_currentTable);
    QString dropOldTableQuery = QString("DROP TABLE %1;").arg(m_currentTable);
    QString renameTableQuery = QString("ALTER TABLE %1 RENAME TO %2;").arg(tempTableName, m_currentTable);

    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, createQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(m_db, copyDataQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(m_db, dropOldTableQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(m_db, renameTableQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

QComboBox* CTableManagerDialog::createTypeComboBox() {
    QComboBox* combo = new QComboBox(this);
    combo->setEditable(true);
    combo->addItems({ "INTEGER", "TEXT", "REAL", "BLOB", "NUMERIC" });
    return combo;
}
