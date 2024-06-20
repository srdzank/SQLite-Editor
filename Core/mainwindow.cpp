#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"
#include <QLineEdit>
#include "TimeLineWidget/customwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG("Application started");

    ui->setupUi(this);
    resize(800, 600);
    layout = new QVBoxLayout();
    setWindowTitle("Editor");
    // Set the window icon
    QIcon appIcon(":/logo.png");
    setWindowIcon(appIcon);

    // Set a background color using stylesheets
    setStyleSheet("QMainWindow { background-color: #767676; }");



    // Create the menu
    CreateMenu();
    createToolBar();
    customWidget =  new CustomWidget(this);
    QMenuBar *menuBar = this->menuBar();
    layout->addWidget(menuBar);
//    customWidget->setGeometry(QRect(0, 20, width()-100, height()/5));

    layout->addWidget(customWidget);
    setLayout(layout);

    // Create the status bar
    this->statusBar()->showMessage("Ready");
    this->statusBar()->setStyleSheet("QStatusBar { background-color: #767676; color: white; }");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateMenu()
{
    // Create the menu bar
    QMenuBar *menuBar = this->menuBar();
    menuBar->setStyleSheet(
        "QMenuBar { background-color: #969696; color: white; }"
        "QMenuBar::item:selected { background-color: red; }"
        "QMenu { background-color: #969696; color: white; }"
        "QMenu::item:selected { background-color: red; }"
        );

    // Create a file menu
    QMenu *fileMenu = menuBar->addMenu("File");
    // Create actions for the file menu
    QAction *openAction = new QAction("Open", this);
    QAction *saveAction = new QAction("Save", this);
    QAction *exitAction = new QAction("Exit", this);

    // Add actions to the file menu
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // Connect the exit action to the close slot of the main window
    connect(exitAction, &QAction::triggered, this, &MainWindow::onActionExit);
    LOG("Main Menu is created");
}


void MainWindow::createToolBar()
{
    // Create a toolbar
    QToolBar *toolBar = addToolBar("Main Toolbar");

    // Create an action for the toolbar
    QAction *customAction = new QAction(QIcon(":/logo.png"), "Custom Button", this);
    connect(customAction, &QAction::triggered, this, &MainWindow::onCustomButtonClicked);

    // Add the action to the toolbar
    toolBar->addAction(customAction);

    // Create a QLineEdit and add it to the toolbar
    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setFixedWidth(200);
    lineEdit->setPlaceholderText("Enter text...");
    //connect(lineEdit, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    toolBar->addWidget(lineEdit);

    // Create an action for the toolbar
    QAction *customAction2 = new QAction(QIcon(":/logo.png"), "Custom Button", this);
    connect(customAction2, &QAction::triggered, this, &MainWindow::onCustomButtonClicked);
    // Add the action to the toolbar
    toolBar->addAction(customAction2);
    addToolBar(Qt::BottomToolBarArea, toolBar);

    LOG("Main ToolBar is created");
}

void MainWindow::onActionExit()
{
    LOG("Application is closed");
    close();
}

void MainWindow::onCustomButtonClicked()
{
    LOG("onCustomButtonClicked is clicked");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    int w = event->size().width();
    int h = event->size().height();
    customWidget->setGeometry(QRect(0,  h-300, w-100, 250));
    QMainWindow::resizeEvent(event);
}
