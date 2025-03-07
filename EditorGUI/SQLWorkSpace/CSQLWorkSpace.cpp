#include "CSQLWorkSpace.h"
#include <QHBoxLayout>
#include <QAbstractItemView>  // Include this for QAbstractItemView

CSQLWorkSpace::CSQLWorkSpace(QWidget* parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    // Create SQL input field
    sqlInput = new AutoCompleteTextEdit(this);

    // Create buttons
    executeButton = new QPushButton("Execute", this);
    clearButton = new QPushButton("Clear", this);
    ERDiagramButton = new QPushButton("ER Diagram", this);

    // Create a horizontal layout for buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(ERDiagramButton);

    // Add widgets to the main layout
    layout->addLayout(buttonLayout);
    layout->addWidget(sqlInput);

    errorLabel = new QLabel(this);  // Initialize the error label
    errorLabel->setStyleSheet("QLabel { color: red; background-color: white; }");  // Set text color and background color
    layout->addWidget(errorLabel);  // Add the error label to the layout


    // Set layout for the widget
    setLayout(layout);

    // Create the completer
    completerModel = new QStringListModel(this);
    completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    sqlInput->setCompleter(completer);

    // Apply styles to the completer
    applyCompleterStyles();  // Add this line

    // Connect buttons to their slots
    connect(executeButton, &QPushButton::clicked, this, &CSQLWorkSpace::onExecuteButtonClicked);
    connect(clearButton, &QPushButton::clicked, this, &CSQLWorkSpace::onClearButtonClicked);
    connect(ERDiagramButton, &QPushButton::clicked, this, &CSQLWorkSpace::onERDiagramClicked);

    errorLabel->hide();
}

void CSQLWorkSpace::onExecuteButtonClicked()
{
    if (sqlInput->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Empty Query"), tr("The SQL query is empty. Please enter a query to execute."));
        return;
    }
    emit executeSQL(sqlInput->toPlainText());
}

void CSQLWorkSpace::onClearButtonClicked()
{
    sqlInput->clear();
    emit clearSQL();
}


void CSQLWorkSpace::onERDiagramClicked()
{
    emit erDiagram();
}

void CSQLWorkSpace::applyCompleterStyles()
{
    completer->popup()->setStyleSheet(
        "QListView {"
        "    background-color: lightyellow;"
        "    border: 1px solid #CCCCCC;"
        "    color: black;"
        "}"
        "QListView::item {"
        "    padding: 4px;"
        "}"
        "QListView::item:selected {"
        "    background-color: darkgray;"
        "    color: white;"
        "}"
    );
}
