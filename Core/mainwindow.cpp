#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"
#include <QLineEdit>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <QStringListModel>
#include <QCompleter>
#include <QHBoxLayout>
#include <iostream>
#include "SQLParserAPI.h"  // Include your SQL parser API implementation
#include "SQLWorkSpace/CSQLWorkSpace.h"  // Include the custom CSQLWorkSpace header

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG("Application started");
    m_db = nullptr;
    erDiagram = nullptr;
    ui->setupUi(this);
    resize(1024, 768);
    setWindowTitle("SQLite Editor");

    // Set the window icon
    QIcon appIcon(":/logo.png");
    setWindowIcon(appIcon);

    // Set a background color using stylesheets
    setStyleSheet("QMainWindow { background-color: #767676; }");

    // Set a central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create the menu
    CreateMenu();
    createToolBar();

    // Set up layout for central widget
    layout = new QVBoxLayout(centralWidget);

    splitter = new QSplitter(Qt::Vertical, this);
    customWidget = new CSQLWorkSpace(this);  // Use the custom CSQLWorkSpace
    tableViewWidget = new CTableViewWorkspace(this);

    splitter->addWidget(customWidget);  // Add the CSQLWorkSpace to the splitter
    splitter->addWidget(tableViewWidget);

    layout->addWidget(splitter);

    // Create a CDBNavigator instance
    navigator = new CDBNavigator(this);
    connect(navigator, SIGNAL(sigClickedTableItem(const QString&)), this, SLOT(procClickedTableItem(const QString&)));

    // Create a dock widget
    QDockWidget* dock = new QDockWidget(tr("Database Navigator"), this);
    dock->setWidget(navigator);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    // Connect signals from customWidget to MainWindow slots
    connect(customWidget, SIGNAL(executeSQL(const QString&)), this, SLOT(onExecuteSQL(const QString&)));
    connect(customWidget, SIGNAL(clearSQL()), this, SLOT(onClearSQL()));
    connect(customWidget, SIGNAL(erDiagram()), this, SLOT(erDiagramProc()));
   

    // Create the completer
    completerModel = new QStringListModel(this);
    completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    customWidget->sqlInput->setCompleter(completer);

    connect(customWidget->sqlInput, &QTextEdit::textChanged, this, &MainWindow::onSQLTextChanged);  // Connect text changed signal

    updateCompleterModel();

    // Create the status bar
    this->statusBar()->showMessage("Copyright (c) by Srdzan Kostenarov");
    this->statusBar()->setStyleSheet("QStatusBar { background-color: #767676; color: white; }");
}



MainWindow::~MainWindow()
{
    // Close the database
    closeDatabase(m_db);
    delete ui;
}


void MainWindow::CreateMenu()
{
    // Create the menu bar
    QMenuBar* menuBar = this->menuBar();
    menuBar->setStyleSheet(
        "QMenuBar { background-color: #969696; color: white; }"
        "QMenuBar::item:selected { background-color: red; }"
        "QMenu { background-color: #969696; color: white; }"
        "QMenu::item:selected { background-color: red; }"
    );

    // Create a file menu
    QMenu* fileMenu = menuBar->addMenu("File");
    fileMenu->setObjectName("File");

    // Create actions for the file menu
    QAction* newDatabaseAction = new QAction("New Database", this);
    QAction* openAction = new QAction("Open Database", this);
    QAction* exitAction = new QAction("Exit", this);

    // Add actions to the file menu
    fileMenu->addAction(newDatabaseAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(newDatabaseAction, &QAction::triggered, this, &MainWindow::onNewDatabase);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(openAction, &QAction::triggered, this, &MainWindow::onActionOpen);

    LOG("Main Menu is created");
}

void MainWindow::onNewDatabase()
{
    // Open a dialog to enter the name and location for the new database
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create New SQLite Database"),
        "", tr("SQLite Database Files (*.sqlite *.db)"));

    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No file name provided.");
        return;
    }

    // Attempt to create the database
    sqlite3* newDb;
    int result = sqlite3_open(fileName.toUtf8().constData(), &newDb);

    if (result != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to create database: %1").arg(sqlite3_errmsg(newDb)));
        sqlite3_close(newDb);
        return;
    }

    // Close any previously opened database
    if (m_db) {
        closeDatabase(m_db);
    }

    // Update the current database
    m_db = newDb;
    QMessageBox::information(this, "Success", "Database created successfully.");

    // Refresh the database navigator
    navigator->openDatabase(m_db);
    if (erDiagram == nullptr) {
        erDiagram = new ERDiagram(m_db, this);
        erDiagram->generateDiagram();
        erDiagram->hide();
        layout->addWidget(erDiagram);
    }
}


void MainWindow::createToolBar()
{
    // Create a toolbar
    QToolBar* toolBar = addToolBar("Main Toolbar");

    // Create an action for the toolbar
    QAction* customAction = new QAction(QIcon(":/logo.png"), "Custom Button", this);
    connect(customAction, &QAction::triggered, this, &MainWindow::onCustomButtonClicked);

    // Add the action to the toolbar
    //toolBar->addAction(customAction);

    // Create a QLineEdit and add it to the toolbar
    //QLineEdit* lineEdit = new QLineEdit(this);
    //lineEdit->setFixedWidth(200);
    //lineEdit->setPlaceholderText("Enter text...");
    //connect(lineEdit, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
//    toolBar->addWidget(lineEdit);

    // Create an action for the toolbar
    QAction* customAction2 = new QAction(QIcon(":/table_create.png"), "Create Table Dialog", this);
    connect(customAction2, &QAction::triggered, this, &MainWindow::onTableCreateDialogClicked);

    QAction* customAction3 = new QAction(QIcon(":/table_edit.png"), "Table Edit Dialog", this);
    connect(customAction3, &QAction::triggered, this, &MainWindow::onTableEditDialogClicked);

    QAction* customAction4 = new QAction(QIcon(":/table_edit.png"), "Get SQL ", this);
    connect(customAction4, &QAction::triggered, this, &MainWindow::onGetSQLQuery);

    // Add the action to the toolbar
    toolBar->addAction(customAction2);
    toolBar->addAction(customAction3);
    toolBar->addAction(customAction4);

    addToolBar(Qt::BottomToolBarArea, toolBar);

    LOG("Main ToolBar is created");
}

void MainWindow::updateLastOpenedFileName(const QString& fileName)
{
    lastOpenedFileName = fileName;
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->findChild<QMenu*>("File");

    if (fileMenu) {
        // Check if there is already an action for the last opened file
        QAction* lastOpenedFileAction = fileMenu->findChild<QAction*>("lastOpenedFileAction");

        if (lastOpenedFileAction) {
            // Update the existing action
            lastOpenedFileAction->setText("Last Opened: " + lastOpenedFileName);
        }
        else {
            // Create a new action for the last opened file
            lastOpenedFileAction = new QAction("Last Opened: " + lastOpenedFileName, this);
            lastOpenedFileAction->setObjectName("lastOpenedFileAction");
            fileMenu->addAction(lastOpenedFileAction);
        }
    }
}

void MainWindow::updateCompleterModel()
{
    QString sqlText = customWidget->sqlInput->toPlainText();
    size_t cursorPosition = customWidget->sqlInput->textCursor().position();

    // Use c_str() to convert std::string to const char*
    void* suggestionsHandle = getSuggestions(sqlText.toStdString().c_str(), cursorPosition, m_db);
    if (!suggestionsHandle) {
        return;
    }

    SQLSuggestions* suggestions = static_cast<SQLSuggestions*>(suggestionsHandle);
    QStringList suggestionList;
    for (const auto& suggestion : suggestions->data) {
        suggestionList << QString::fromStdString(suggestion);
    }
    completerModel->setStringList(suggestionList);

    // Free the suggestions handle
    freeSuggestions(suggestionsHandle);
}

void MainWindow::onSQLTextChanged()
{
    updateCompleterModel();
}

void MainWindow::printResults(const std::vector<std::vector<std::string>>& results,
    const std::vector<std::string>& columnNames)
{
    tableViewWidget->printResults(results, columnNames);
}

void MainWindow::onActionExit()
{
    LOG("Application is closed");
    close();
}

void MainWindow::onActionOpen()
{
    LOG("File is open");
    if (m_db) {
        delete erDiagram;
        closeDatabase(m_db);
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open SQLite Database"),
        "",
        tr("SQLite Database Files (*.sqlite *.db)"));
    if (!fileName.isEmpty()) {
        QByteArray byteArray = fileName.toUtf8();
        const char* filename = byteArray.constData();

        if (openDatabase(filename, &m_db) != SQLITE_OK) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;

            return;
        }
        navigator->openDatabase(m_db);
        QString sql = "SELECT * FROM users;";

        updateLastOpenedFileName(fileName); // Add this line to update the menu
        // Inside your main window or a dedicated widget
        
        erDiagram = new ERDiagram(m_db, this);
        erDiagram->generateDiagram();
        erDiagram->hide();
        layout->addWidget(erDiagram);
    }
}


void MainWindow::executeSQLCommand(sqlite3* db, const QString& eSql) {
    customWidget->errorLabel->hide();
    customWidget->errorLabel->setText("");
    QByteArray byteArray = eSql.toUtf8();
    const char* sql = byteArray.constData();

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        customWidget->errorLabel->show();
        customWidget->errorLabel->setText("Error: " + (QString)sqlite3_errmsg(db));
        return;
    }

    int cols = sqlite3_column_count(stmt);
    std::vector<std::string> columnNames;
    for (int i = 0; i < cols; i++) {
        columnNames.push_back(sqlite3_column_name(stmt, i));
    }

    std::vector<std::vector<std::string>> resultData;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int col = 0; col < cols; col++) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
            row.push_back(text ? text : "NULL");
        }
        resultData.push_back(row);
    }

    sqlite3_finalize(stmt);

    // Print the results
    printResults(resultData, columnNames);
}


void MainWindow::procClickedTableItem(const QString& table) {
    m_table = table;
    QString sql = "SELECT * FROM " + table + " LIMIT 10;";
    executeSQLCommand(m_db, sql);

    std::cout << "Database operations completed successfully." << std::endl;
}

void MainWindow::onExecuteSQL(const QString& query)
{
    executeSQLCommand(m_db, query);  // Assuming table name is "results"
}

void MainWindow::onClearSQL()
{
    customWidget->sqlInput->clear();
}


void MainWindow::erDiagramProc()
{
    if (erDiagram == nullptr) return;

    if (erDiagram->isVisible()) {
        erDiagram->hide();
        tableViewWidget->show();
    }
    else {
        delete erDiagram;
        if(m_db){
            erDiagram = new ERDiagram(m_db, this);
            erDiagram->generateDiagram();
            erDiagram->show();
            layout->addWidget(erDiagram);
            tableViewWidget->hide();
        }
    }
}

void MainWindow::onCustomButtonClicked()
{
    LOG("onCustomButtonClicked is clicked");

}


void MainWindow::onTableCreateDialogClicked()
{
    LOG("onCustomButtonClicked is clicked");
    CTableManagerDialog  dialog(m_db, this); // Pass the SQLite handle
    dialog.exec();
    navigator->openDatabase(m_db);
}

void MainWindow::onTableEditDialogClicked()
{
    LOG("onCustomButtonClicked is clicked");
    tableDiagram = new TableDiagram(m_db, this);
    tableDiagram->loadDatabaseSchema();
    // Add to your layout
    QGraphicsView* view = new QGraphicsView(tableDiagram);
    view->show();
    layout->addWidget(view);

}

void MainWindow::onGetSQLQuery()
{
    if (tableDiagram != nullptr){
        QString sqlString = tableDiagram->generateSelectSQL();
        customWidget->sqlInput->setPlainText(sqlString);
    }
}


void MainWindow::resizeEvent(QResizeEvent* event)
{

    QMainWindow::resizeEvent(event);
}
