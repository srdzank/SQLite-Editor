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

    manageIndexesButton = new QPushButton("Manage Indexes", this);
    actionButtonLayout->addWidget(manageIndexesButton);
    connect(manageIndexesButton, &QPushButton::clicked, this, &CTableManagerDialog::manageIndexes);

    // Foreign Key Management UI
    foreignKeyTable = new QTableWidget(this);
    foreignKeyTable->setColumnCount(3);
    foreignKeyTable->setHorizontalHeaderLabels({ "Foreign Key Column", "Referenced Table", "Referenced Column" });
    foreignKeyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    addForeignKeyButton = new QPushButton("Add Foreign Key", this);
    removeForeignKeyButton = new QPushButton("Remove Foreign Key", this);

    QHBoxLayout* foreignKeyButtonLayout = new QHBoxLayout();
    foreignKeyButtonLayout->addWidget(addForeignKeyButton);
    foreignKeyButtonLayout->addWidget(removeForeignKeyButton);

    QVBoxLayout* foreignKeyLayout = new QVBoxLayout();
    foreignKeyLayout->addWidget(foreignKeyTable);
    foreignKeyLayout->addLayout(foreignKeyButtonLayout);

    mainLayout->addLayout(foreignKeyLayout);

    // Connect signals for foreign key management
    connect(addForeignKeyButton, &QPushButton::clicked, this, &CTableManagerDialog::addForeignKey);
    connect(removeForeignKeyButton, &QPushButton::clicked, this, &CTableManagerDialog::removeForeignKey);

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
    foreignKeyTable->setRowCount(0); // Clear foreign key table

    if (tableName.isEmpty()) {
        return;
    }

    // Load columns using PRAGMA table_info
    QString query = QString("PRAGMA table_info(%1);").arg(tableName);
    sqlite3_stmt* stmt;

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

    // Load foreign keys using PRAGMA foreign_key_list
    QString fkQuery = QString("PRAGMA foreign_key_list(%1);").arg(tableName);
    if (sqlite3_prepare_v2(m_db, fkQuery.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to load foreign keys.");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int row = foreignKeyTable->rowCount();
        foreignKeyTable->insertRow(row);

        foreignKeyTable->setItem(row, 0, new QTableWidgetItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))));
        foreignKeyTable->setItem(row, 1, new QTableWidgetItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
        foreignKeyTable->setItem(row, 2, new QTableWidgetItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))));
    }

    sqlite3_finalize(stmt);
}


void CTableManagerDialog::saveTableChanges() {
    if (alterTable()) {
        QMessageBox::information(this, "Success", "Table changes saved successfully, including foreign keys.");
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

    // Add foreign key constraints
    QStringList foreignKeyDefinitions;
    for (int i = 0; i < foreignKeyTable->rowCount(); ++i) {
        QString foreignKeyColumn = foreignKeyTable->item(i, 0)->text();
        QString referencedTable = foreignKeyTable->item(i, 1)->text();
        QString referencedColumn = foreignKeyTable->item(i, 2)->text();

        QString foreignKeyDef = QString("FOREIGN KEY (%1) REFERENCES %2(%3)")
            .arg(foreignKeyColumn)
            .arg(referencedTable)
            .arg(referencedColumn);

        foreignKeyDefinitions.append(foreignKeyDef);
    }

    queryStr += columnDefinitions.join(", ");
    if (!foreignKeyDefinitions.isEmpty()) {
        queryStr += ", " + foreignKeyDefinitions.join(", ");
    }

    queryStr += ");";
    return queryStr;
}


bool CTableManagerDialog::alterTable() {
    if (m_currentTable.isEmpty()) {
        QMessageBox::critical(this, "Error", "No table selected for modification.");
        return false;
    }

    // Confirm deletion and recreation
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Modify Table",
        QString("Modifying the table '%1' will delete its data. Do you want to proceed?").arg(m_currentTable),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return false; // User canceled
    }

    // Step 1: Drop the old table
    QString dropTableQuery = QString("DROP TABLE IF EXISTS %1;").arg(m_currentTable);
    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, dropTableQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to delete the old table: %1").arg(errMsg));
        sqlite3_free(errMsg);
        return false;
    }

    // Step 2: Create the new table
    QString createTableQuery = generateCreateTableQuery(m_currentTable);
    if (sqlite3_exec(m_db, createTableQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to create the new table: %1").arg(errMsg));
        sqlite3_free(errMsg);
        return false;
    }

    return true; // Successfully modified the table
}


QComboBox* CTableManagerDialog::createTypeComboBox() {
    QComboBox* combo = new QComboBox(this);
    combo->setEditable(true);
    combo->addItems({ "INTEGER", "TEXT", "REAL", "BLOB", "NUMERIC" });
    return combo;
}

void CTableManagerDialog::manageIndexes() {
    if (m_currentTable.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No table selected to manage indexes.");
        return;
    }

    QDialog indexDialog(this);
    indexDialog.setWindowTitle(QString("Manage Indexes for '%1'").arg(m_currentTable));
    indexDialog.resize(700, 400);

    QVBoxLayout* layout = new QVBoxLayout(&indexDialog);

    QLabel* indexesLabel = new QLabel(QString("Indexes for table '%1':").arg(m_currentTable));
    QTableWidget* indexesTable = new QTableWidget(&indexDialog);
    indexesTable->setColumnCount(2);
    indexesTable->setHorizontalHeaderLabels({ "Index Name", "Indexed Columns" });
    indexesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPushButton* addIndexButton = new QPushButton("Add Index", &indexDialog);
    QPushButton* deleteIndexButton = new QPushButton("Delete Index", &indexDialog);
    QPushButton* closeButton = new QPushButton("Close", &indexDialog);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addIndexButton);
    buttonLayout->addWidget(deleteIndexButton);
    buttonLayout->addWidget(closeButton);

    layout->addWidget(indexesLabel);
    layout->addWidget(indexesTable);
    layout->addLayout(buttonLayout);

    connect(addIndexButton, &QPushButton::clicked, this, [=]() {
        createIndex();
        populateIndexes(indexesTable);
        });

    connect(deleteIndexButton, &QPushButton::clicked, this, [=]() {
        deleteIndex();
        populateIndexes(indexesTable);
        });

    connect(closeButton, &QPushButton::clicked, &indexDialog, &QDialog::accept);

    populateIndexes(indexesTable);

    indexDialog.exec(); // Open the dialog modally
}

void CTableManagerDialog::populateIndexes(QTableWidget* indexesTable) {
    indexesTable->clearContents();
    indexesTable->setRowCount(0);

    QString query = QString("PRAGMA index_list('%1');").arg(m_currentTable);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, query.toUtf8().constData(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int row = indexesTable->rowCount();
            indexesTable->insertRow(row);

            QString indexName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            indexesTable->setItem(row, 0, new QTableWidgetItem(indexName));

            // Get indexed columns
            QString columnQuery = QString("PRAGMA index_info('%1');").arg(indexName);
            sqlite3_stmt* columnStmt;
            QStringList indexedColumns;
            if (sqlite3_prepare_v2(m_db, columnQuery.toUtf8().constData(), -1, &columnStmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(columnStmt) == SQLITE_ROW) {
                    indexedColumns.append(reinterpret_cast<const char*>(sqlite3_column_text(columnStmt, 2)));
                }
                sqlite3_finalize(columnStmt);
            }
            indexesTable->setItem(row, 1, new QTableWidgetItem(indexedColumns.join(", ")));
        }
        sqlite3_finalize(stmt);
    }
}

void CTableManagerDialog::createIndex() {
    bool ok;
    QString indexName = QInputDialog::getText(this, "Add Index", "Index Name:", QLineEdit::Normal, "", &ok);
    if (!ok || indexName.isEmpty()) return;

    QString columns = QInputDialog::getText(this, "Add Index", "Comma-separated columns to index:", QLineEdit::Normal, "", &ok);
    if (!ok || columns.isEmpty()) return;

    QString createIndexQuery = QString("CREATE INDEX IF NOT EXISTS %1 ON %2 (%3);")
        .arg(indexName)
        .arg(m_currentTable)
        .arg(columns);
    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, createIndexQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to create index: %1").arg(errMsg));
        sqlite3_free(errMsg);
    }
    else {
        QMessageBox::information(this, "Success", "Index created successfully.");
    }
}


void CTableManagerDialog::deleteIndex() {
    bool ok;
    QString indexName = QInputDialog::getText(this, "Delete Index", "Enter the name of the index to delete:", QLineEdit::Normal, "", &ok);
    if (!ok || indexName.isEmpty()) return;

    QString dropIndexQuery = QString("DROP INDEX IF EXISTS %1;").arg(indexName);
    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, dropIndexQuery.toUtf8().constData(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to delete index: %1").arg(errMsg));
        sqlite3_free(errMsg);
    }
    else {
        QMessageBox::information(this, "Success", "Index deleted successfully.");
    }
}

void CTableManagerDialog::addForeignKey() {
    int row = foreignKeyTable->rowCount();
    foreignKeyTable->insertRow(row);

    // Foreign Key Column
    QTableWidgetItem* foreignKeyColumnItem = new QTableWidgetItem();
    foreignKeyTable->setItem(row, 0, foreignKeyColumnItem);

    // Referenced Table
    QComboBox* referencedTableCombo = new QComboBox(this);

    // Populate the combo box with table names from `tableSelector`
    for (int i = 0; i < tableSelector->count(); ++i) {
        referencedTableCombo->addItem(tableSelector->itemText(i));
    }
    foreignKeyTable->setCellWidget(row, 1, referencedTableCombo);

    // Referenced Column
    QTableWidgetItem* referencedColumnItem = new QTableWidgetItem();
    foreignKeyTable->setItem(row, 2, referencedColumnItem);
}


void CTableManagerDialog::removeForeignKey() {
    int currentRow = foreignKeyTable->currentRow();
    if (currentRow >= 0) {
        foreignKeyTable->removeRow(currentRow);
    }
}
