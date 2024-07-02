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

    // Create a horizontal layout for buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(clearButton);

    // Add widgets to the main layout
    layout->addLayout(buttonLayout);
    layout->addWidget(sqlInput);

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
