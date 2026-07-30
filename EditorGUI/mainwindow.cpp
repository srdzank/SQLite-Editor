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
#include <QApplication>
#include <iostream>
#include "SQLParserAPI.h"
#include "SQLWorkSpace/CSQLWorkSpace.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG("Application started");
    m_db = nullptr;
    erDiagram = nullptr;
    tableDiagram = nullptr;
    view = nullptr;

    ui->setupUi(this);
    resize(1024, 768);
    setWindowTitle("SQLite Editor");

    // Set the window icon
    QIcon appIcon(":/Res/logo.png");
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
    customWidget = new CSQLWorkSpace(this);
    tableViewWidget = new CTableViewWorkspace(this);

    splitter->addWidget(customWidget);
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

    connect(customWidget->sqlInput, &QTextEdit::textChanged, this, &MainWindow::onSQLTextChanged);

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
    QAction* settingsAction = new QAction("Settings", this);
    QAction* exitAction = new QAction("Exit", this);

    // Add actions to the file menu
    fileMenu->addAction(newDatabaseAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(newDatabaseAction, &QAction::triggered, this, &MainWindow::onNewDatabase);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(openAction, &QAction::triggered, this, &MainWindow::onActionOpen);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);

    LOG("Main Menu is created");
}

void MainWindow::onNewDatabase()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create New SQLite Database"),
        "", tr("SQLite Database Files (*.sqlite *.db)"));

    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No file name provided.");
        return;
    }

    sqlite3* newDb;
    int result = sqlite3_open(fileName.toUtf8().constData(), &newDb);

    if (result != SQLITE_OK) {
        QMessageBox::critical(this, "Error", QString("Failed to create database: %1").arg(sqlite3_errmsg(newDb)));
        sqlite3_close(newDb);
        return;
    }

    if (m_db) {
        if (erDiagram) {
            delete erDiagram;
            erDiagram = nullptr;
        }
        if (view) {
            delete view;
            view = nullptr;
            tableDiagram = nullptr;
        }
        closeDatabase(m_db);
    }

    m_db = newDb;
    QMessageBox::information(this, "Success", "Database created successfully.");

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
    QToolBar* toolBar = addToolBar("Main Toolbar");

    QAction* customAction = new QAction(QIcon(":/Res/logo.png"), "Custom Button", this);
    connect(customAction, &QAction::triggered, this, &MainWindow::onCustomButtonClicked);

    QAction* customAction2 = new QAction(QIcon(":/Res/table_create.png"), "Create Table Dialog", this);
    connect(customAction2, &QAction::triggered, this, &MainWindow::onTableCreateDialogClicked);

    QAction* customAction3 = new QAction(QIcon(":/Res/table_edit.png"), "SQL-SELECT-Wizard", this);
    connect(customAction3, &QAction::triggered, this, &MainWindow::onTableEditDialogClicked);

    QAction* customAction4 = new QAction(QIcon(":/Res/get_sql.png"), "Get SQL ", this);
    connect(customAction4, &QAction::triggered, this, &MainWindow::onGetSQLQuery);

    QAction* settingsAction = new QAction("Settings", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);

    toolBar->addAction(customAction2);
    toolBar->addAction(customAction3);
    toolBar->addAction(customAction4);
    toolBar->addAction(settingsAction);

    // Поставување на Toolbar-от горе
    addToolBar(Qt::TopToolBarArea, toolBar);

    LOG("Main ToolBar is created");
}

void MainWindow::updateLastOpenedFileName(const QString& fileName)
{
    lastOpenedFileName = fileName;
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->findChild<QMenu*>("File");

    if (fileMenu) {
        QAction* lastOpenedFileAction = fileMenu->findChild<QAction*>("lastOpenedFileAction");

        if (lastOpenedFileAction) {
            lastOpenedFileAction->setText("Last Opened: " + lastOpenedFileName);
        }
        else {
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
        if (erDiagram) {
            delete erDiagram;
            erDiagram = nullptr;
        }
        if (view) {
            delete view;
            view = nullptr;
            tableDiagram = nullptr;
        }
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

        updateLastOpenedFileName(fileName);

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
    executeSQLCommand(m_db, query);

    if (view != nullptr) {
        delete view;
        view = nullptr;
        tableDiagram = nullptr;
        if (splitter) {
            splitter->show();
        }
    }
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
        if (m_db) {
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
    CTableManagerDialog dialog(m_db, this);
    dialog.exec();
    navigator->openDatabase(m_db);
}

void MainWindow::onTableEditDialogClicked()
{
    LOG("SQL-SELECT-Wizard action triggered");

    if (view != nullptr) {
        if (view->isVisible()) {
            view->hide();
            if (splitter) {
                splitter->show();
            }
        }
        else {
            if (splitter) {
                splitter->hide();
            }
            view->show();
        }
        return;
    }

    if (!m_db) {
        QMessageBox::warning(this, "Warning", "No database opened.");
        return;
    }

    tableDiagram = new TableDiagram(m_db, this);
    tableDiagram->loadDatabaseSchema();

    view = new QGraphicsView(tableDiagram, this);

    if (splitter) {
        splitter->hide();
    }

    layout->addWidget(view);
    view->show();
}

void MainWindow::onGetSQLQuery()
{
    if (tableDiagram != nullptr) {
        QString sqlString = tableDiagram->generateSelectSQL();
        customWidget->sqlInput->setPlainText(sqlString);

        if (view != nullptr && view->isVisible()) {
            view->hide();
            if (splitter) {
                splitter->show();
            }
        }
    }
}

void MainWindow::onSettingsClicked()
{
    CSettingsDialog dlg(qApp->font(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QFont newFont = dlg.getSelectedFont();

        // 1. Постави глобален фонт
        qApp->setFont(newFont);

        // 2. Примени го експлицитно на сите виџети кои се веќе креирани
        const auto allWidgets = qApp->allWidgets();
        for (QWidget* w : allWidgets) {
            w->setFont(newFont);
            w->update();
        }

        // 3. Доколку користиш специфичен stylesheet во main.cpp или customWidget (QTextEdit)
        // исто така осигури се дека и текстот во SQL Input-от се ажурира
        if (customWidget && customWidget->sqlInput) {
            customWidget->sqlInput->setFont(newFont);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}