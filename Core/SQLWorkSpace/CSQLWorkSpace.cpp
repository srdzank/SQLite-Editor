#include "CSQLWorkSpace.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

CSQLWorkSpace::CSQLWorkSpace(QWidget* parent)
    : QWidget(parent)
{
    // Create the QTextEdit for SQL command input
    sqlTextEdit = new QTextEdit(this);
    sqlTextEdit->setPlaceholderText("Write your SQL command here...");

    // Create the buttons
    executeButton = new QPushButton("Execute", this);
    clearButton = new QPushButton("Clear", this);

    // Connect the buttons to their respective slots
    connect(executeButton, &QPushButton::clicked, this, &CSQLWorkSpace::onExecuteClicked);
    connect(clearButton, &QPushButton::clicked, this, &CSQLWorkSpace::onClearClicked);

    // Create a layout to arrange the buttons horizontally
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(clearButton);

    // Create the main layout to arrange the QTextEdit and the button layout vertically
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(sqlTextEdit);

    // Set the layout for the widget
    setLayout(mainLayout);
}

CSQLWorkSpace::~CSQLWorkSpace()
{
}

void CSQLWorkSpace::onExecuteClicked()
{
    QString sqlCommand = sqlTextEdit->toPlainText();
    if (sqlCommand.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("SQL command is empty!"));
        return;
    }

    // Execute the SQL command here
    // For demonstration, we'll just show a message box
    QMessageBox::information(this, tr("Execute"), tr("Executing SQL command:\n%1").arg(sqlCommand));
}

void CSQLWorkSpace::onClearClicked()
{
    sqlTextEdit->clear();
}
